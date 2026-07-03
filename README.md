# Simple Shell

A Unix-like command shell implemented from scratch in C++, built to understand process management, file descriptors, and inter-process communication at the OS level.

## Features

- **Built-in commands**: `cd` (with `HOME` fallback and error handling), `pwd`, `exit` (with custom exit status codes)
- **I/O redirection**: `<` (input), `>` (output, truncate), `>>` (output, append)
- **Piping**: supports chaining any number of commands (`cmd1 | cmd2 | cmd3 | ...`), not just a single pipe
- **Error handling**: invalid commands, missing files, malformed redirection syntax, and empty commands are caught and reported without crashing the shell

## Project Structure

The codebase is split into separate header/source files by responsibility, rather than living in one file:

```
main.cpp               — the shell's main input loop and command dispatch logic
shell_builtins.h/.cpp  — built-in commands that must run in the shell's own process (cd, pwd)
shell_utils.h/.cpp     — parsing utilities (splitting input on '|' into separate commands)
```

Each `.h` file declares function signatures only; the matching `.cpp` file holds the actual implementation. `main.cpp` includes the headers it needs and calls into them, without knowing (or needing to know) the implementation details — a basic form of separating interface from implementation.

## How It Works

The shell reads a line of input, tokenizes it, and dispatches it in one of three ways:

1. **Built-ins** (`cd`, `pwd`, `exit`) run directly in the shell process — no `fork()` needed, since they must affect the shell's own state (e.g. `cd` changes the shell's working directory).
2. **Single commands** (with optional `<`, `>`, `>>`) are run in a forked child process. The child sets up any file redirections via `dup2()` before calling `execvp()` to replace itself with the target program.
3. **Piped commands** (`cmd1 | cmd2 | ... | cmdN`) are split into N separate commands. The shell creates N−1 pipes and forks N child processes, wiring each child's stdin/stdout to the appropriate pipe ends so data flows from one command to the next — the same mechanism real shells use.

### Key OS concepts demonstrated
- `fork()` — process creation and duplication
- `execvp()` — replacing a process image with a new program
- `pipe()` — unidirectional inter-process communication
- `dup2()` — file descriptor redirection
- `wait()` / `waitpid()` — process synchronization and zombie prevention
- Proper file descriptor cleanup to avoid pipe deadlocks/hangs

## Build & Run

```bash
g++ -o shell main.cpp shell_builtins.cpp shell_utils.cpp
./shell
```

All three `.cpp` files must be compiled together, since `main.cpp` only contains declarations (via the headers) for the builtins and utility functions — their actual definitions live in the other two `.cpp` files and need to be linked in.

## Usage Examples

```
shell> pwd
/home/user/simple-shell

shell> cd ..
shell> ls > output.txt
shell> cat < output.txt
shell> echo "log entry" >> output.txt

shell> ls | grep .cpp | wc -l
shell> cat main.cpp | grep include | sort

shell> exit 0
```

## Known Limitations

- No support for background execution (`&`), job control, or signal handling (e.g. `Ctrl+C` forwarding)
- No environment variable expansion (`$VAR`) or wildcard globbing (`*.txt`)
- No command history

These were intentionally scoped out to focus on core process/IPC mechanics; they're natural next steps for extending the project.

## Tech Stack

C++ · POSIX system calls (`unistd.h`, `sys/wait.h`, `fcntl.h`) · WSL/Linux