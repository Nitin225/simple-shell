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