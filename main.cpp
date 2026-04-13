#include<iostream>
#include<string>
#include <sstream>
#include <vector>
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

        if (input == "exit") break;
        cout << "Command: " << args[0] << endl;
        cout << "Args count: " << args.size() << endl;


    }


}