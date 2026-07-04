#ifndef SHELL_BUILTINS_H
#define SHELL_BUILTINS_H

#include <vector>
#include <string>
using namespace std;

void builtinCd(const vector<string>& args);
void builtinPwd();


#endif