#include<iostream>
#include<string>
using namespace std;

int main(){
    while(true){
        string input;
        cout<< "shell> ";
        getline(cin, input);
        if (input == "exit") break;
        cout << input << endl;
    }
}