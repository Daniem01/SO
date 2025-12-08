# 🐧 Operating Systems Internals (SSOO)

Comprehensive collection of system programming labs for the **Operating Systems** course at **Complutense University of Madrid (UCM)**.

This repository focuses on **low-level interaction with the Linux Kernel**, utilizing the **POSIX API** for process management, file systems, concurrency, and kernel modules.

## 📂 Project Structure

| Project | Focus Area | Key Concepts & Syscalls |
| :--- | :--- | :--- |
| **[p2-c-fundamentals](./p2-c-fundamentals)** | **System Programming Basis** | Advanced C standard library usage, I/O streams, and memory management basics. |
| **[p3-file-system](./p3-file-system)** | **POSIX File System** | Low-level file manipulation. <br> `open`, `read`, `write`, `lstat`, `symlink`, `opendir`. |
| **[p4-processes-threads](./p4-processes-threads)** | **Process Management** | Process/Thread lifecycle and file descriptor inheritance. <br> `fork`, `exec`, `pthread_create`, `wait`. |
| **[p5-thread-sync](./p5-thread-sync)** | **Thread Synchronization** | Solving race conditions in multithreaded environments. <br> `pthread_mutex`, `pthread_cond_wait`, `monitors`. |
| **[p6-ipc-mechanisms](./p6-ipc-mechanisms)** | **Inter-Process Communication (IPC)** | Synchronization between distinct processes using Shared Memory. <br> `sem_open`, `mmap`, `shm_open`, `sem_wait/post`. |
| **[p7-linux-drivers](./p7-linux-drivers)** | **Kernel Modules** | Introduction to **Linux Device Drivers**, kernel space programming, and module insertion. |

## 🛠️ Technologies & Tools

* **Language:** C (C99 Standard)
* **OS:** GNU/Linux (POSIX Standard)
* **Tools:** GCC, Make, GDB, Valgrind
* **Kernel:** Linux Kernel API (for Drivers)

## 🚀 How to Build

Each practice contains a dedicated `Makefile` for compilation.

1.  Navigate to the specific exercise directory:
    ```bash
    cd p3-file-system/exercise1
    ```
2.  Compile the source code:
    ```bash
    make all
    ```
3.  Clean binaries:
    ```bash
    make clean
    ```

## ⚖️ License
This project is licensed under the [MIT License](LICENSE.txt).
