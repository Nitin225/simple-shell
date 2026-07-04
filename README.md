# Simple Shell

A Unix-like command shell built in C++ using low-level system calls (`fork`, `execvp`, `pipe`, `dup2`).

## Features

- **Command execution** — runs any executable via `fork()` + `execvp()`
- **Built-in commands** — `cd` (with `HOME` fallback), `pwd`, `exit`
- **Multi-stage pipes** — supports chaining any number of commands: `cmd1 | cmd2 | cmd3`
- **I/O redirection** — `<` (input), `>` (output, truncate), `>>` (output, append)
- **Combined pipes + redirection** — e.g. `grep foo < in.txt | sort > out.txt`
- **Error handling** — invalid commands, missing files, and malformed syntax fail gracefully instead of crashing

## Build

```bash
g++ -o shell main.cpp shell_builtins.cpp shell_utils.cpp
./shell
```

## Usage

```bash
shell> ls | grep .cpp | wc -l
shell> cat < input.txt > output.txt
shell> cd ..
shell> exit
```

## Project Structure

| File | Responsibility |
|---|---|
| `main.cpp` | REPL loop, input parsing, pipe/process orchestration |
| `shell_builtins.cpp/h` | Built-in commands (`cd`, `pwd`) |
| `shell_utils.cpp/h` | Pipe splitting and redirection parsing |

## How it works

1. Input is tokenized into arguments.
2. Tokens are split into pipeline stages on `|`.
3. Each stage is parsed independently for `<`, `>`, `>>` redirection.
4. For N stages, N-1 pipes are created; each stage forks a child process, wires its stdin/stdout via `dup2`, and `execvp`s the command.
5. The parent closes all pipe file descriptors and waits for every child before showing the next prompt.