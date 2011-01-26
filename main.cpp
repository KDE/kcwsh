#include <iostream>
#include <string>
#include <vector>

#include <windows.h>

#include "clienthandler.h"

using namespace std;

void usage() {
    cout << "a" << endl \
         << "b" << endl \
         << "c" << endl \
         << "d" << endl \
         << "e" << endl;
}
int main(int argc, char **argv) {
    vector<string> args(argv, argv + argc);

    cout << "kcwsh - Windows wrapper shell" << endl;
    for(vector<string>::const_iterator it = args.begin(); it != args.end(); it++) {
        if(*it == "-h" || *it == "--help") {
            usage();
            return 1;
        }
    }
    ClientHandler handler("C:\\Windows\\SysWOW64\\cmd.exe");
    
    cout << "Starting process: " << ((handler.start()) ? "succeeded" : "failed") << endl;
    cout << "Injecting Dll: " << ((handler.inject()) ? "succeeded" : "failed") << endl;
    
    return 0;
}