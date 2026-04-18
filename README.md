# Simple Shell

A Unix shell implemented in C++ from scratch.

## Features
- Command execution (ls, pwd, cat, etc.)
- Built-in commands: `cd`, `exit`
- Output redirection (`ls > file.txt`)
- Piping (`ls | grep main`)
- Error handling for invalid commands

## How to Run
```bash
g++ main.cpp -o shell
./shell
```

## Tech
- C++
- Linux system calls: fork, exec, pipe, dup2, wait