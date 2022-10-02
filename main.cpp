#include <iostream>
#include "command.h"
using namespace std;

int main(){
    string cLine;

    while (1) {
        cout << "shell379 > ";
        getline(cin, cLine);
        if (!cLine.empty()) {
            process_command(cLine);
        }
    }

}