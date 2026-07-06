# Linux System Programming

A structured collection of experiments, notes, and small implementations documenting my exploration of Linux system programming from first principles.

## Description

This repository is not a tutorial, a course solution set, or a production library. It is a personal record of hands-on work with Linux internals: processes, signals, files, IPC, sockets, namespaces, cgroups, I/O multiplexing, and memory management. Each directory represents a focused, minimal program written to answer a specific question about how Linux behaves at the system call level.

The code here favors clarity over cleverness and correctness of understanding over completeness of features. Where something surprised me, I wrote it down.

## Motivation

Most of my day-to-day work as a software engineering student happens several layers above the kernel. Frameworks, runtimes, and libraries abstract away scheduling, memory layout, and I/O — which is convenient, but it also means the actual mechanics are easy to take for granted.

I started this repository to close that gap deliberately: to write small C/C++ programs that exercise the raw system call interface, observe what actually happens (using tools like `strace`, `ltrace`, `/proc`, and `gdb`), and build an accurate mental model of the OS instead of an assumed one.

## Goals

- Build working knowledge of the Linux system call interface through direct use, not just documentation.
- Understand process lifecycle, scheduling, and resource management well enough to reason about them without guessing.
- Develop comfort with low-level debugging and observability tools (`strace`, `perf`, `/proc`, `gdb`).
- Establish a foundation for larger systems projects (e.g. container runtimes, custom schedulers, network services) built on verified understanding rather than copied patterns.
- Keep an honest, dated trail of what I tried, what worked, what didn't, and why.

## Repository Structure

```text
linux-system-programming/
├── README.md
├── process/
│   ├── fork/              # fork() semantics, copy-on-write, PID behavior
│   ├── wait/               # wait()/waitpid(), reaping, exit status handling
│   ├── exec/               # exec() family, image replacement, environment
│   └── zombie-process/     # zombie states, orphaned processes, reaping edge cases
├── signals/                # signal delivery, handlers, masking, async-signal-safety
├── files/                  # file descriptors, low-level I/O, permissions, links
├── pipes/                  # anonymous/named pipes, blocking behavior, FIFOs
├── sockets/                # Unix domain sockets, TCP/UDP basics, socket options
├── namespaces/              # PID/mount/network/UTS namespace isolation experiments
├── cgroups/                 # cgroups v2 controllers, resource limiting
├── epoll/                   # epoll vs select/poll, edge-triggered vs level-triggered I/O
└── memory/                   # virtual memory, mmap, page faults, allocator behavior
```

Each experiment directory generally contains:

- **Source code** — a small, self-contained C or C++ program targeting one concept.
- **A local README** — explaining the concept being tested and what the program does.
- **Observations** — what actually happened when the code ran, including output from tools like `strace` or `/proc` inspection.
- **References** — relevant man page sections (`man 2`, `man 7`) and other primary sources consulted.
- **Notes on edge cases** — anything unexpected, undocumented-but-observed, or platform-dependent.

## Topics Covered

- Process creation, termination, and reaping (`fork`, `exec`, `wait`, zombie/orphan states)
- Signal handling and delivery semantics
- File descriptor management and low-level file I/O
- Inter-process communication via pipes and Unix domain sockets
- Basic network programming with sockets
- Linux namespaces as an isolation primitive
- cgroups v2 for resource accounting and limiting
- I/O multiplexing with `epoll`
- Virtual memory concepts: `mmap`, page faults, address space layout

## Learning Philosophy

The approach here is deliberately narrow and repeatable:

1. Pick one system call or kernel-provided mechanism.
2. Write the smallest program that exercises it meaningfully.
3. Observe actual behavior using tracing and inspection tools rather than relying on assumptions from documentation alone.
4. Record what happened, especially where it diverged from expectation.
5. Only then move on to the next concept, or revisit this one under different conditions (e.g. under load, with errors injected, across different kernel versions).

This is intentionally slow and repetitive. The goal is an accurate internal model of Linux behavior, not a fast survey of APIs.

## Current Progress

This repository is under active, ongoing development. Coverage varies by topic — some directories (e.g. `process/fork`, `process/wait`) contain multiple worked experiments with detailed notes, while others (e.g. `cgroups`, `epoll`) are newer and still sparse. Each directory's local README reflects its actual current state rather than a planned one.

## How to Build and Run Examples

Most examples are single-file C or C++ programs with no external dependencies beyond a standard toolchain.

```bash
# Clone the repository
git clone https://github.com/<your-username>/linux-system-programming.git
cd linux-system-programming

# Build a specific example (adjust path/filename as needed)
cd process/fork
gcc -Wall -Wextra -o fork_demo fork_demo.c
./fork_demo

# Trace system calls for a given example
strace -f ./fork_demo
```

Compiler flags and any special build requirements (e.g. `-pthread`, specific kernel version dependencies) are noted in the relevant experiment's local README.

## Recommended References

- Linux man pages — sections 2 (system calls) and 7 (conventions/overview), accessed via `man 2 <syscall>` or [man7.org](https://man7.org/linux/man-pages/)
- Michael Kerrisk, *The Linux Programming Interface* (TLPI)
- Linux kernel documentation — [kernel.org/doc](https://www.kernel.org/doc/html/latest/)
- W. Richard Stevens, *Advanced Programming in the UNIX Environment*
- Robert Love, *Linux System Programming*
- `Documentation/admin-guide/cgroup-v2.rst` in the kernel source tree
- `namespaces(7)`, `pid_namespaces(7)`, `cgroups(7)` man pages

## Future Topics

- seccomp-bpf filtering
- Rootless container primitives (user namespaces, UID/GID mapping)
- Custom minimal container runtime combining namespaces, cgroups, and seccomp
- Asynchronous I/O (`io_uring`)
- Basic scheduler behavior and `sched_setattr`
- Shared memory and futex-based synchronization

## Contributing

This is primarily a personal learning log, but corrections, alternative explanations, and technical suggestions are genuinely welcome — especially where an observation is incomplete or a stated behavior turns out to be kernel-version-specific or otherwise inaccurate. Feel free to open an issue or pull request.

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.
