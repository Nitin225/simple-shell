#include "shell_utils.h"

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

ParsedCommand parseRedirection(const vector<string>& tokens) {
    ParsedCommand result;
    for (size_t i = 0; i < tokens.size(); i++) {
        if (tokens[i] == ">" || tokens[i] == ">>") {
            result.appendMode = (tokens[i] == ">>");
            if (i + 1 >= tokens.size()) {
                cerr << "shell: syntax error: expected filename after " << tokens[i] << "\n";
                result.syntaxError = true;
                continue;
            }
            result.outfile = tokens[i+1];
            i++;
            continue;
        }
        if (tokens[i] == "<") {
            if (i + 1 >= tokens.size()) {
                cerr << "shell: syntax error: expected filename after <\n";
                result.syntaxError = true;
                continue;
            }
            result.infile = tokens[i+1];
            i++;
            continue;
        }
        result.cmd_args.push_back(tokens[i]);
    }
    return result;
}