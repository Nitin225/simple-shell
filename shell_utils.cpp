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