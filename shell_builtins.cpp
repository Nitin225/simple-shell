#include "shell_builtins.h"
#include <unistd.h>
#include <cstdlib>
#include <iostream>

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