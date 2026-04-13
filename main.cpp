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

        if (input == "exit") break;
        pid_t pip = fork();
        if (pip == 0) {
            execlp(args[0].c_str(), args[0].c_str(), nullptr);
        }
        else {
            wait(nullptr);
        }



    }


}