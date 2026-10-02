#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <dirent.h>
#include <unistd.h>
#include <cctype>
#include <iomanip>

using namespace std;

struct ProcessInfo
{
    int pid;
    string name;
    long memoryKB;
    unsigned long cpuTime;
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
            stringstream ss(line);

            string label;
            long memory;
            string unit;

            ss >> label >> memory >> unit;

            return memory;
        }
    }

    return 0;
}

unsigned long getCPUTime(const string& pid)
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

int main()
{
    cout << "\n";
    cout << "============================================================\n";
    cout << "                  LINUX PROCESS GUARDIAN\n";
    cout << "============================================================\n";
    cout << "\n";

    const long MEMORY_THRESHOLD = 500000;

    DIR* directory = opendir("/proc");

    if (directory == nullptr)
    {
        cerr << "ERROR: Unable to access /proc\n";
        return 1;
    }

    vector<ProcessInfo> processes;

    struct dirent* entry;

    while ((entry = readdir(directory)) != nullptr)
    {
        string pidText = entry->d_name;

        if (!isNumber(pidText))
            continue;

        int pid = stoi(pidText);

        ProcessInfo process;

        process.pid = pid;
        process.name = getProcessName(pidText);
        process.memoryKB = getMemoryUsage(pidText);
        process.cpuTime = getCPUTime(pidText);

        processes.push_back(process);
    }

    closedir(directory);

    cout << left
         << setw(8) << "PID"
         << setw(32) << "PROCESS"
         << setw(15) << "MEMORY"
         << setw(15) << "CPU TIME"
         << "\n";

    cout << "------------------------------------------------------------\n";

    int count = 0;

    for (const auto& process : processes)
    {
        cout << left
             << setw(8) << process.pid
             << setw(32) << process.name.substr(0, 30)
             << setw(15) << to_string(process.memoryKB) + " KB"
             << setw(15) << process.cpuTime
             << "\n";

        count++;

        if (count >= 20)
            break;
    }

    cout << "------------------------------------------------------------\n";

    cout << "\nSystem Information\n";
    cout << "------------------\n";

    cout << "Memory threshold : "
         << MEMORY_THRESHOLD
         << " KB\n";

    cout << "Processes found  : "
         << processes.size()
         << "\n";

    bool warningFound = false;

    int warningPID = 0;
    string warningName;
    long warningMemory = 0;

    for (const auto& process : processes)
    {
        if (process.memoryKB >= MEMORY_THRESHOLD)
        {
            warningFound = true;

            if (process.memoryKB > warningMemory)
            {
                warningPID = process.pid;
                warningName = process.name;
                warningMemory = process.memoryKB;
            }
        }
    }

    cout << "\nSystem Status\n";
    cout << "-------------\n";

    if (warningFound)
    {
        cout << "WARNING: High memory usage detected.\n";

        cout << "Process : " << warningName << "\n";
        cout << "PID     : " << warningPID << "\n";
        cout << "Memory  : " << warningMemory << " KB\n";
    }
    else
    {
        cout << "NORMAL: No process exceeded the memory threshold.\n";
    }

    cout << "\n============================================================\n";
    cout << "              Process scan completed.\n";
    cout << "============================================================\n";
    cout << "\n";

    return 0;
}
