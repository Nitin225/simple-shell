#include<iostream>
#include<string>
#include<sstream>
#include<vector>
#include<unistd.h>
#include<sys/wait.h>
#include <fcntl.h>
#include<cstring>
#include <cstdlib>
using namespace std;

void builtinCd(const vector<string>& args) {
    string target;
    if (args.size() < 2) {
        const char* home = getenv("HOME");
        if (!home) {
            cerr << "shell: cd: HOME not set\n";
            return;
        }
        target = home;
    } else {
        target = args[1];
    }

    if (chdir(target.c_str()) != 0) {
        cerr << "shell: cd: " << target << ": No such file or directory\n";
    }
}

void builtinPwd() {
    char buf[4096];
    if (getcwd(buf, sizeof(buf)) != nullptr) {
        cout << buf << "\n";
    } else {
        perror("shell: pwd");
    }
}


vector<vector<string>> splitByPipe(const vector<string>& args) {
    vector<vector<string>> result;      
    vector<string> current;            

    for (int i = 0; i < args.size(); i++) {
        if (args[i] == "|") {
            result.push_back(current);
            current.clear();
        } else {
            current.push_back(args[i]);
        }
    }

    result.push_back(current);

    return result;
}



int main(){
    while(true){
        string input;
        string token;
        vector<string> args;
        cout<< "shell> ";
        getline(cin, input);
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


        string infile = "";
        bool appendMode = false;
        string outfile = "";
        vector<string> cmd_args;

        for (int i = 0; i < args.size(); i++) {
            if (args[i] == ">" || args[i] == ">>") {
                appendMode = (args[i] == ">>");
                if (i + 1 >= args.size()) {
                    cerr << "shell: syntax error: expected filename after " << args[i] << "\n";
                    continue;
                }
                outfile = args[i+1];
                i++;             
                continue;
            }
            if (args[i] == "<") {
                if (i + 1 >= args.size()) {
                    cerr << "shell: syntax error: expected filename after " << args[i] << "\n";
                    continue;
                }
                infile = args[i+1];
                i++;             
                continue;
            }
            cmd_args.push_back(args[i]);
        }


        vector<vector<string>> commands = splitByPipe(args);
        if(commands.size() > 1){
            int n = commands.size();
            vector<vector<int>> pipes(n-1, vector<int>(2));
            for(int i=0; i<n-1; i++){
                if(pipe(pipes[i].data()) < 0){
                    perror("pipes");
                    exit(1);
                }
            }
            vector<pid_t> pids;

            for (int i = 0; i < n; i++) {
                pid_t pid = fork();
                if (pid == 0) {
                
                    if (i > 0) {
                        dup2(pipes[i-1][0], STDIN_FILENO);
                    }
                    if (i < n - 1) {
                        dup2(pipes[i][1], STDOUT_FILENO);
                    }
                    for (int j = 0; j < n - 1; j++) {
                        close(pipes[j][0]);
                        close(pipes[j][1]);
                    }
                    vector<char*> cargs;
                    for (auto& arg : commands[i]) cargs.push_back(&arg[0]);
                    cargs.push_back(nullptr);
                    execvp(cargs[0], cargs.data());

                    cerr << "shell: command not found: " << commands[i][0] << "\n";
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
            continue;
        }




        if(cmd_args.empty()){
            cerr << "shell: syntax error: no command given\n";
            continue;
        }

        pid_t pip = fork();
        if (pip == 0) {
            if (!infile.empty()) {
                int fdIn = open(infile.c_str(), O_RDONLY);
                if (fdIn < 0) {
                    cerr << "shell: " << infile << ": No such file or directory\n";
                    exit(1);
                }
                dup2(fdIn, STDIN_FILENO);
                close(fdIn);
            }

            if (!outfile.empty()) {
                int flags = O_WRONLY | O_CREAT | (appendMode ? O_APPEND : O_TRUNC);
                int fd = open(outfile.c_str(), flags, 0644);
                if (fd < 0) {
                    cerr << "shell: cannot open " << outfile << ": for writings\n";
                    exit(1);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }

            vector<char*> cargs;
            for (auto& arg : cmd_args) {
                cargs.push_back(&arg[0]);
            }
            cargs.push_back(nullptr); 
            execvp(cargs[0], cargs.data());
            cerr << "Command invalid: " << args[0] << endl;
            exit(1);
        }
        else {
            wait(nullptr);
        }
    }
}