#include<iostream>
#include<string>
#include<sstream>
#include<vector>
#include<unistd.h>
#include<sys/wait.h>
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
        pid_t pip = fork();
        if (pip == 0) {
            vector<char*> cargs;
            for (auto& arg : args) {
                cargs.push_back(&arg[0]);
            }
            cargs.push_back(nullptr); // array ka end
            execvp(cargs[0], cargs.data());
            cerr << "Command invalid: " << args[0] << endl;
            exit(1);  
        }
        else {
            wait(nullptr);
        }

    }


}