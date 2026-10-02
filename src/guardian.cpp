#include <iostream>
#include <fstream>
#include <string>
#include <dirent.h>
#include <cctype>

using namespace std;

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
        {
            return line.substr(6);
        }
    }

    return "Unknown";
}

int main()
{
    cout << "========================================\n";
    cout << "       LINUX PROCESS GUARDIAN\n";
    cout << "========================================\n\n";

    DIR* directory = opendir("/proc");

    if (directory == nullptr)
    {
        cerr << "Error: Cannot access /proc\n";
        return 1;
    }

    struct dirent* entry;

    cout << "PID\tPROCESS\n";
    cout << "----------------------------------------\n";

    while ((entry = readdir(directory)) != nullptr)
    {
        string pid = entry->d_name;

        if (!isNumber(pid))
            continue;

        string processName = getProcessName(pid);

        cout << pid << "\t" << processName << "\n";
    }

    closedir(directory);

    cout << "\n----------------------------------------\n";
    cout << "Process scan completed.\n";
    cout << "========================================\n";

    return 0;
}
