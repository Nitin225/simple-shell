#include<iostream>
#include<string>
#include<sstream>
#include<vector>
#include<unistd.h>
#include<sys/wait.h>
#include <fcntl.h>
#include<cstring>
using namespace std;

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
        chdir(args[1].c_str());
        continue;
        }

        if (args[0] == "exit") break;

        string outfile = "";
        vector<string> cmd_args;

        for (int i = 0; i < args.size(); i++) {
            if (args[i] == ">") {
                outfile = args[i+1];
                break;
            }
            cmd_args.push_back(args[i]);
        }

        string pipe_cmd = "";
        vector<string> left_args, right_args;
        bool has_pipe = false;

        for (int i = 0; i < args.size(); i++) {
            if (args[i] == "|") {
                has_pipe = true;
                for (int j = i+1; j < args.size(); j++) {
                    right_args.push_back(args[j]);
                }
                break;
            }
            left_args.push_back(args[i]);
        }

        if (has_pipe) {
        int pipefd[2];
        pipe(pipefd);
        
        pid_t left = fork();
        if (left == 0) {
            dup2(pipefd[1], STDOUT_FILENO); 
            close(pipefd[0]);
            close(pipefd[1]);
            vector<char*> cargs;
            for (auto& arg : left_args) cargs.push_back(&arg[0]);
            cargs.push_back(nullptr);
            execvp(cargs[0], cargs.data());
            exit(1);
        }
        
        pid_t right = fork();
        if (right == 0) {
            dup2(pipefd[0], STDIN_FILENO); 
            close(pipefd[0]);
            close(pipefd[1]);
            vector<char*> cargs;
            for (auto& arg : right_args) cargs.push_back(&arg[0]);
            cargs.push_back(nullptr);
            execvp(cargs[0], cargs.data());
            exit(1);
        }
        
        close(pipefd[0]);
        close(pipefd[1]);
        wait(nullptr);
        wait(nullptr);
        continue;
    }

        pid_t pip = fork();
        if (pip == 0) {
            if (!outfile.empty()) {
            int fd = open(outfile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
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