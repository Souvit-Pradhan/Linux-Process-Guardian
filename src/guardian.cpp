#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <dirent.h>
#include <unistd.h>
#include <cctype>
#include <iomanip>
#include <thread>
#include <chrono>
#include <fcntl.h>

using namespace std;

struct ProcessInfo
{
    int pid;
    string name;
    long memoryKB;
    unsigned long cpuTime;
    double cpuPercent;
};

bool isNumber(const string& text)
{
    if (text.empty())
        return false;

    for (char c : text)
    {
        if (!isdigit(static_cast<unsigned char>(c)))
            return false;
    }

    return true;
}

string getProcessName(const string& pid)
{
    string path = "/proc/" + pid + "/status";
    ifstream file(path);

    if (!file)
        return "Unknown";

    string line;

    while (getline(file, line))
    {
        if (line.rfind("Name:", 0) == 0)
            return line.substr(6);
    }

    return "Unknown";
}

long getMemoryUsage(const string& pid)
{
    string path = "/proc/" + pid + "/status";
    ifstream file(path);

    if (!file)
        return 0;

    string line;

    while (getline(file, line))
    {
        if (line.rfind("VmRSS:", 0) == 0)
        {
            string label;
            long memory;
            string unit;

            stringstream ss(line);

            ss >> label >> memory >> unit;

            return memory;
        }
    }

    return 0;
}

unsigned long getProcessCPUTime(const string& pid)
{
    string path = "/proc/" + pid + "/stat";
    ifstream file(path);

    if (!file)
        return 0;

    string line;

    getline(file, line);

    stringstream ss(line);

    string value;
    vector<string> fields;

    while (ss >> value)
        fields.push_back(value);

    if (fields.size() < 17)
        return 0;

    unsigned long userTime = stoul(fields[13]);
    unsigned long kernelTime = stoul(fields[14]);

    return userTime + kernelTime;
}

unsigned long getTotalCPUTime()
{
    ifstream file("/proc/stat");

    if (!file)
        return 0;

    string line;

    while (getline(file, line))
    {
        if (line.rfind("cpu ", 0) == 0)
        {
            string cpu;

            unsigned long user;
            unsigned long nice;
            unsigned long system;
            unsigned long idle;
            unsigned long iowait;
            unsigned long irq;
            unsigned long softirq;
            unsigned long steal;

            stringstream ss(line);

            ss >> cpu
               >> user
               >> nice
               >> system
               >> idle
               >> iowait
               >> irq
               >> softirq
               >> steal;

            return user + nice + system + idle +
                   iowait + irq + softirq + steal;
        }
    }

    return 0;
}

vector<ProcessInfo> getProcesses()
{
    vector<ProcessInfo> processes;

    DIR* directory = opendir("/proc");

    if (directory == nullptr)
        return processes;

    struct dirent* entry;

    while ((entry = readdir(directory)) != nullptr)
    {
        string pidText = entry->d_name;

        if (!isNumber(pidText))
            continue;

        ProcessInfo process;

        process.pid = stoi(pidText);
        process.name = getProcessName(pidText);
        process.memoryKB = getMemoryUsage(pidText);
        process.cpuTime = getProcessCPUTime(pidText);
        process.cpuPercent = 0.0;

        processes.push_back(process);
    }

    closedir(directory);

    return processes;
}

string getDriverStatus()
{
    int fd = open("/dev/procguard", O_RDONLY);

    if (fd < 0)
    {
        return "NOT CONNECTED";
    }

    char buffer[128] = {0};

    ssize_t bytesRead = read(fd, buffer, sizeof(buffer) - 1);

    close(fd);

    if (bytesRead <= 0)
    {
        return "NO RESPONSE";
    }

    return string(buffer);
}

int main()
{
    cout << "\n";
    cout << "============================================================\n";
    cout << "                  LINUX PROCESS GUARDIAN\n";
    cout << "============================================================\n";
    cout << "\n";

    const long MEMORY_THRESHOLD = 500000;
    const double CPU_THRESHOLD = 80.0;
    string driverStatus = getDriverStatus();

    cout << "Driver Status    : "
         << driverStatus;
    cout << "Collecting CPU information...\n";
    cout << "Please wait 1 second...\n\n";

    vector<ProcessInfo> firstSnapshot = getProcesses();

    unsigned long firstTotalCPU = getTotalCPUTime();

    this_thread::sleep_for(chrono::seconds(1));

    vector<ProcessInfo> secondSnapshot = getProcesses();

    unsigned long secondTotalCPU = getTotalCPUTime();

    unsigned long totalCPUDifference =
        secondTotalCPU - firstTotalCPU;

    for (auto& process : secondSnapshot)
    {
        for (const auto& oldProcess : firstSnapshot)
        {
            if (process.pid == oldProcess.pid)
            {
                unsigned long processDifference =
                    process.cpuTime - oldProcess.cpuTime;

                if (totalCPUDifference > 0)
                {
                    process.cpuPercent =
                        (static_cast<double>(processDifference) /
                         static_cast<double>(totalCPUDifference)) *
                        100.0;
                }

                break;
            }
        }
    }

    cout << left
         << setw(8) << "PID"
         << setw(32) << "PROCESS"
         << setw(12) << "CPU %"
         << setw(15) << "MEMORY"
         << "\n";

    cout << "------------------------------------------------------------\n";

    int count = 0;

    bool warningFound = false;

    int warningPID = 0;
    string warningName;
    double warningCPU = 0.0;
    long warningMemory = 0;

    for (const auto& process : secondSnapshot)
    {
        cout << left
             << setw(8) << process.pid
             << setw(32) << process.name.substr(0, 30)
             << setw(12) << fixed << setprecision(2)
             << process.cpuPercent
             << setw(15)
             << to_string(process.memoryKB) + " KB";

        if (process.cpuPercent >= CPU_THRESHOLD)
        {
            cout << "  [CPU WARNING]";
        }
        else if (process.memoryKB >= MEMORY_THRESHOLD)
        {
            cout << "  [MEMORY WARNING]";
        }

        cout << "\n";

        if (process.cpuPercent >= CPU_THRESHOLD ||
            process.memoryKB >= MEMORY_THRESHOLD)
        {
            warningFound = true;

            if (process.cpuPercent >= CPU_THRESHOLD)
            {
                warningPID = process.pid;
                warningName = process.name;
                warningCPU = process.cpuPercent;
                warningMemory = process.memoryKB;
            }
            else if (process.memoryKB > warningMemory)
            {
                warningPID = process.pid;
                warningName = process.name;
                warningCPU = process.cpuPercent;
                warningMemory = process.memoryKB;
            }
        }

        count++;

        if (count >= 20)
            break;
    }

    cout << "------------------------------------------------------------\n";

    cout << "\nSystem Configuration\n";
    cout << "--------------------\n";

    cout << "CPU threshold     : "
         << CPU_THRESHOLD << "%\n";

    cout << "Memory threshold  : "
         << MEMORY_THRESHOLD << " KB\n";

    cout << "Processes found   : "
         << secondSnapshot.size() << "\n";

    cout << "\nSystem Status\n";
    cout << "-------------\n";

    if (warningFound)
    {
        cout << "WARNING: Resource threshold exceeded.\n";
        cout << "Process : " << warningName << "\n";
        cout << "PID     : " << warningPID << "\n";
        cout << "CPU     : " << fixed << setprecision(2)
             << warningCPU << "%\n";
        cout << "Memory  : " << warningMemory << " KB\n";
    }
    else
    {
        cout << "NORMAL: No process exceeded the configured thresholds.\n";
    }

    cout << "\n============================================================\n";
    cout << "              Process scan completed.\n";
    cout << "============================================================\n";
    cout << "\n";

    return 0;
}
