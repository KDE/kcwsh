#include <iostream>
#include <string>
#include <vector>

#include <windows.h>

#include "clienthandler.h"
#include "pipehandler.h"

using namespace std;

void usage() {
    cout << "kcwsh is a wrapper Shell for cmd.exe which understands ansi " << endl \
         << "escape sequences for some functionality." << endl \
         << endl << "options:" << endl \
         << "-h, --help" << "\tDisplays this help message." << endl \
         << "-d, --debug" << "\tswitches on debug mode" << endl \
         << endl << "Copyright (C) by Patrick Spendrin 2011" << endl \
         << "Redistributable under GPLv3" << endl;
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
    PipeHandler in(PipeHandler::STDIN_PIPE);
    PipeHandler out(PipeHandler::STDOUT_PIPE);
    PipeHandler err(PipeHandler::STDERR_PIPE);
    
    cout << "Starting process: " << ((handler.start(in.readHandle(), out.writeHandle(), err.writeHandle())) ? "succeeded" : "failed") << endl;
    cout << "Injecting Dll: " << ((handler.inject()) ? "succeeded" : "failed") << endl;
    
    return 0;
}