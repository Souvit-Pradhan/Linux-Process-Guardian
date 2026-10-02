# Linux Process Guardian

Linux Process Guardian is a lightweight Linux system-monitoring application developed in C++ with a custom Linux kernel character device driver.

The project monitors running processes, CPU usage, memory usage, and system memory. It also communicates with a custom kernel driver through `/dev/procguard`.

## Features

- Lists running Linux processes
- Displays Process ID (PID)
- Displays process name
- Calculates CPU usage
- Displays process memory usage
- Sorts processes by CPU usage
- Identifies the highest CPU-consuming process
- Identifies the highest memory-consuming process
- Provides configurable CPU and memory thresholds
- Generates CPU and memory warnings
- Displays overall system memory usage
- Continuously refreshes monitoring information
- Communicates with a custom Linux kernel character device driver
- Displays kernel driver connection status

## Project Architecture

```text
+-----------------------------+
|     C++ Process Monitor     |
|        guardian.cpp         |
+--------------+--------------+
               |
               | open/read
               v
+-----------------------------+
|       /dev/procguard        |
+--------------+--------------+
               |
               v
+-----------------------------+
|    Linux Kernel Driver      |
|        procguard.c          |
+-----------------------------+
               |
               v
+-----------------------------+
|        Linux Kernel         |
|   Process & System Data     |
+-----------------------------+
