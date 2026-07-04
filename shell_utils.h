#ifndef SHELL_UTILS_H
#define SHELL_UTILS_H

#include<iostream>
#include <vector>
#include <string>
using namespace std;

vector<vector<string>> splitByPipe(const vector<string>& arg);

struct ParsedCommand {
    vector<string> cmd_args;
    string infile = "";
    string outfile = "";
    bool appendMode = false;
    bool syntaxError = false;
};

ParsedCommand parseRedirection(const vector<string>& tokens);


#endif