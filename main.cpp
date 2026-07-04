#include<iostream>
#include<string>
#include<sstream>
#include<vector>
#include<unistd.h>
#include<sys/wait.h>
#include <fcntl.h>
#include<cstring>
#include <cstdlib>
#include "shell_builtins.h"
#include "shell_utils.h"
using namespace std;

int main(){
    while(true){
        string input;
        string token;
        vector<string> args;
        cout << "shell> ";
        if (!getline(cin, input)) break;   // handles Ctrl+D / EOF cleanly
        istringstream iss(input);

        while(iss >> token){
            args.push_back(token);
        }
        if (args.empty()) continue;

        if (args[0] == "cd") {
            builtinCd(args);
            continue;
        }

        if (args[0] == "pwd") {
            builtinPwd();
            continue;
        }

        if (args[0] == "exit") {
            int code = 0;
            if (args.size() >= 2) {
                code = atoi(args[1].c_str());
            }
            exit(code);
        }

        vector<vector<string>> commands = splitByPipe(args);
        int n = commands.size();

        vector<ParsedCommand> parsed(n);
        bool anyError = false;
        for (int i = 0; i < n; i++) {
            parsed[i] = parseRedirection(commands[i]);
            if (parsed[i].syntaxError) anyError = true;
            if (parsed[i].cmd_args.empty()) {
                cerr << "shell: syntax error: no command given\n";
                anyError = true;
            }
        }
        if (anyError) continue;

        vector<vector<int>> pipes(n > 1 ? n - 1 : 0, vector<int>(2));
        for (int i = 0; i < n - 1; i++) {
            if (pipe(pipes[i].data()) < 0) {
                perror("pipe");
                exit(1);
            }
        }

        vector<pid_t> pids;
        for (int i = 0; i < n; i++) {
            pid_t pid = fork();
            if (pid == 0) {
                if (i > 0) dup2(pipes[i-1][0], STDIN_FILENO);
                if (i < n - 1) dup2(pipes[i][1], STDOUT_FILENO);
                for (int j = 0; j < n - 1; j++) {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }

                if (!parsed[i].infile.empty()) {
                    int fdIn = open(parsed[i].infile.c_str(), O_RDONLY);
                    if (fdIn < 0) {
                        cerr << "shell: " << parsed[i].infile << ": No such file or directory\n";
                        exit(1);
                    }
                    dup2(fdIn, STDIN_FILENO);
                    close(fdIn);
                }

                if (!parsed[i].outfile.empty()) {
                    int flags = O_WRONLY | O_CREAT | (parsed[i].appendMode ? O_APPEND : O_TRUNC);
                    int fd = open(parsed[i].outfile.c_str(), flags, 0644);
                    if (fd < 0) {
                        cerr << "shell: cannot open " << parsed[i].outfile << " for writing\n";
                        exit(1);
                    }
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                }

                vector<char*> cargs;
                for (auto& arg : parsed[i].cmd_args) cargs.push_back(&arg[0]);
                cargs.push_back(nullptr);
                execvp(cargs[0], cargs.data());

                cerr << "shell: command not found: " << parsed[i].cmd_args[0] << "\n";
                exit(1);
            }
            pids.push_back(pid);
        }

        for (int j = 0; j < n - 1; j++) {
            close(pipes[j][0]);
            close(pipes[j][1]);
        }
        for (int j = 0; j < n; j++) {
            waitpid(pids[j], nullptr, 0);
        }
    }
}