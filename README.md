#  Linux Process Guardian

A lightweight Linux process monitoring system that combines a **C++ user-space application** with a **Linux kernel character device driver** to monitor system processes, CPU usage, memory usage, and resource threshold violations.

The project demonstrates practical Linux system programming concepts such as `/proc` filesystem monitoring, process information extraction, CPU usage calculation, memory monitoring, kernel modules, character devices, user-space/kernel-space communication, and continuous system monitoring.

---

##  Overview

Modern Linux systems run many processes simultaneously, and some processes may consume excessive CPU or memory resources.

**Linux Process Guardian** provides a simple terminal-based monitoring solution that:

- Detects running processes
- Monitors CPU usage
- Monitors memory usage
- Sorts processes according to CPU consumption
- Detects processes crossing configured resource thresholds
- Displays system resource information
- Communicates with a Linux kernel driver
- Continuously refreshes the monitoring information

The project is designed to be lightweight, easy to understand, and suitable for demonstrating Linux system programming concepts.

---

#  Problem Statement

Linux systems may run hundreds of processes at the same time. Identifying processes that consume excessive CPU or memory can be difficult using only basic system commands.

There is a need for a lightweight monitoring tool that can:

1. Discover running processes.
2. Calculate CPU usage.
3. Monitor process memory consumption.
4. Identify processes exceeding resource limits.
5. Provide clear warnings.
6. Demonstrate communication between user space and kernel space.

---

#  Proposed Solution

Linux Process Guardian solves this problem using two main components:

### 1. User-Space Process Monitor

The C++ application:

- Scans the `/proc` filesystem.
- Identifies running process IDs.
- Reads process names.
- Reads memory usage.
- Calculates CPU usage.
- Sorts processes by CPU consumption.
- Detects threshold violations.
- Displays monitoring information in the terminal.
- Communicates with the kernel driver through `/dev/procguard`.

### 2. Linux Kernel Driver

The kernel module:

- Registers a Linux miscellaneous character device.
- Creates `/dev/procguard`.
- Provides a driver status message.
- Handles device open/read/close operations.
- Logs driver activity using the Linux kernel logging system.

The user-space application verifies the driver connection by reading from `/dev/procguard`.

---

#  Features

- 🔍 Automatic process discovery
- 📊 Real-time CPU usage monitoring
- 💾 Process memory monitoring
- ⚠️ CPU threshold detection
- ⚠️ Memory threshold detection
- 🔝 CPU-based process sorting
- 📈 System memory usage information
- 🧩 Linux kernel driver integration
- 🔗 User-space/kernel-space communication
- 🔄 Continuous monitoring mode
- 🖥️ Simple terminal-based interface
- 📝 Kernel driver activity logging
- 🛠️ Makefile-based compilation
- 🌐 Git/GitHub version control

---

#  System Architecture

```text
                         LINUX OPERATING SYSTEM
                                  │
                 ┌────────────────┴────────────────┐
                 │                                 │
                 ▼                                 ▼
        ┌──────────────────┐              ┌──────────────────┐
        │   Linux /proc    │              │  Kernel Module   │
        │    Filesystem    │              │   procguard.c    │
        └────────┬─────────┘              └────────┬─────────┘
                 │                                 │
                 │ Process Information             │ Character Device
                 │                                 │
                 ▼                                 ▼
        ┌─────────────────────────────────────────────────────┐
        │                C++ Process Guardian                 │
        │                    guardian.cpp                     │
        │                                                     │
        │  • Process Discovery                               │
        │  • CPU Usage Calculation                           │
        │  • Memory Monitoring                               │
        │  • Threshold Detection                             │
        │  • Process Sorting                                 │
        │  • Driver Status Verification                      │
        └────────────────────────┬────────────────────────────┘
                                 │
                                 │ Reads
                                 ▼
                        ┌──────────────────┐
                        │  /dev/procguard  │
                        └────────┬─────────┘
                                 │
                                 ▼
                        ┌──────────────────┐
                        │ Kernel Driver    │
                        │ Status Message   │
                        └──────────────────┘
                                 │
                                 ▼
                     ┌─────────────────────────┐
                     │   Terminal Monitoring   │
                     │                         │
                     │ CPU Usage               │
                     │ Memory Usage            │
                     │ Warnings                │
                     │ Driver Status           │
                     └─────────────────────────┘
