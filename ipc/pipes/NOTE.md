# Linux Pipes — Reference Notes

## 1. Core Facts

- A pipe is a **unidirectional** byte stream with two file descriptors: `fd[0]` (read end) and `fd[1]` (write end).
- Pipes have **no message boundaries** — it's a raw byte stream, not discrete packets. Multiple writes can be coalesced into one read, and one write can be split across multiple reads.
- Backed by a **kernel buffer** (default 64 KB on Linux, tunable via `fcntl(fd, F_SETPIPE_SZ, size)`).
- Two kinds:
  - **Anonymous pipe** (`pipe()`) — no filesystem entry, only accessible to related processes (parent/child via `fork()`, or threads).
  - **Named pipe / FIFO** (`mkfifo()`) — has a path in the filesystem, so unrelated processes can open it by name.
- A pipe's fds are just like any other fd — they survive `fork()` (duplicated into the child) and can be passed over Unix domain sockets via `SCM_RIGHTS`.
- `read()` on a pipe returns `0` (EOF) only when **all** copies of the write end are closed.
- `write()` on a pipe with no open read end raises `SIGPIPE` (default action: terminate process) and the syscall returns `-1` with `errno = EPIPE` if you've ignored/handled the signal.

## 2. Rules & Constraints (must obey)

1. **Close unused ends immediately, in every process.**
   After `fork()`, both parent and child have both ends open. Each process must close the end it doesn't use — otherwise EOF never propagates, because the kernel only signals EOF when *all* write-end copies are closed.
   - Child using `fd[0]` to read → child closes `fd[1]`.
   - Parent using `fd[1]` to write → parent closes `fd[0]`.

2. **Always check `close()`'s return value against `-1`, not the fd against `-1` inside the call.**
   `close(fd == -1)` is a classic bug — it evaluates the boolean first, silently closing fd `0` or `1`. Write `if (close(fd) == -1)`.

3. **Terminate each `fork()` branch explicitly.**
   `switch(fork())` (or plain `if`) has no implicit break between branches. Missing `exit()`/`return` at the end of the child's branch means it falls through into code meant for the parent — double-closes, double-writes, corrupted state.

4. **Never assume a single `read()`/`write()` transfers all the bytes you asked for.**
   Both can return fewer bytes than requested (short read/write), especially with pipes whose kernel buffer is smaller than your data, or on signal interruption (`EINTR`). Always loop until you've transferred the full amount or hit EOF/error.

5. **Don't read and write from the same process on a pipe you're also forking across without a clear protocol**, or you risk deadlock — e.g. child blocks on `read()` waiting for data while parent blocks on `write()` waiting for buffer space, and neither side ever proceeds if buffer size is exceeded and no one is draining it.

6. **Handle or ignore `SIGPIPE` deliberately if writing to a pipe whose reader might exit early.**
   Default behavior kills your process silently. Either:
   - `signal(SIGPIPE, SIG_IGN)` and check `write()`'s return for `EPIPE`, or
   - use `MSG_NOSIGNAL` (sockets only, not plain pipes).

7. **A pipe's capacity is finite (kernel buffer, default 64 KB).**
   A `write()` larger than the free buffer space will block (or partially write) until a reader drains it. Don't assume writes are non-blocking or atomic beyond `PIPE_BUF` (see below).

8. **Writes of size ≤ `PIPE_BUF`** (typically 4096 bytes on Linux, defined in `<limits.h>`) **are atomic** when multiple writers share a pipe — the kernel won't interleave them. Writes larger than `PIPE_BUF` can be interleaved with other writers' data.

9. **FIFOs block on `open()` until both ends are opened**, unless you pass `O_NONBLOCK`. A reader opening a FIFO with no writer (blocking mode) waits until a writer shows up, and vice versa.

## 3. Best Practices

- **Set up the pipe before `fork()`**, never after — both processes need to inherit the same fds.
- **Close both ends in a process as soon as it's fully done**, not just the unused end at the start — don't let stray copies of fds linger and prevent EOF from ever being seen by the other side.
- **Wrap read/write loops to handle partial transfers**:
  ```c
  ssize_t n, total = 0;
  while (total < len) {
      n = read(fd, buf + total, len - total);
      if (n == -1) {
          if (errno == EINTR) continue;
          perror("read"); break;
      }
      if (n == 0) break; // EOF
      total += n;
  }
  ```
- **Use `dup2()` to redirect a pipe end onto STDIN_FILENO/STDOUT_FILENO** when chaining into `exec()` — this is how shells build `cmd1 | cmd2`.
- **Prefer non-blocking mode (`O_NONBLOCK`) combined with `select()`/`poll()`/`epoll()`** if you need to multiplex several pipes or avoid blocking indefinitely on one.
- **For anything needing bidirectional communication, use two pipes (or a socketpair) — not one.** A single pipe is one-way only; trying to make it two-way invites confusion and deadlock.
- **Prefer `socketpair(AF_UNIX, SOCK_STREAM, 0, fds)` over two plain pipes** when you want full-duplex communication between related processes — it's simpler than managing two separate pipe pairs.
- **Always reap the child (`wait()`/`waitpid()`)** after the pipe communication finishes, to avoid zombie processes.
- **Don't rely on pipe data being null-terminated or line-delimited** unless your protocol explicitly adds delimiters — pipes carry raw bytes only.
