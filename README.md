# Linux Process Guardian

A lightweight Linux system-monitoring application that monitors running processes, tracks CPU and memory usage, detects resource-intensive processes, and communicates with a custom Linux kernel driver.

## Features

- Real-time process monitoring
- CPU usage monitoring
- Memory usage monitoring
- CPU and memory threshold warnings
- Top CPU-consuming process detection
- Top memory-consuming process detection
- Continuous monitoring with automatic refresh
- Custom Linux kernel character-device driver
- User-space and kernel-space communication
- Driver status verification through `/dev/procguard`
- Kernel logging using `dmesg`

## Technologies

- C++17
- C
- Linux / Debian
- Linux `/proc` filesystem
- Linux Kernel Module
- Character Device Driver
- g++
- Make
- Git & GitHub

## Architecture

```text
             Linux Process Guardian
                      |
          +-----------+-----------+
          |                       |
          ↓                       ↓
     Linux /proc             Kernel Driver
          |                       |
          ↓                       ↓
 Process Information       /dev/procguard
          |                       |
          +-----------+-----------+
                      |
                      ↓
              C++ Monitoring App
                      |
          +-----------+-----------+
          |           |           |
          ↓           ↓           ↓
        CPU %      Memory     Threshold
                   Usage       Detection
          |           |           |
          +-----------+-----------+
                      |
                      ↓
               System Status
