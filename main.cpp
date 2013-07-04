#include <iostream>
#include <string>
#include <vector>

#include <windows.h>
#include <Shlobj.h>

#include "clienthandler.h"
#include <kcwapp.h>

#define BUFSIZE 4096

static bool g_debug = false;

void usage() {
    std::cout << "kcwsh is a wrapper Shell for cmd.exe which understands ansi " << std::endl \
         << "escape sequences for some functionality." << std::endl \
         << std::endl << "options:" << std::endl \
         << "-h, --help" << "\tDisplays this help message." << std::endl \
         << "-d, --debug" << "\tswitches on debug mode" << std::endl \
         << std::endl << "Copyright (C) by Patrick Spendrin 2011" << std::endl \
         << "Redistributable under GPLv3" << std::endl;
}

std::string getDefaultCmdInterpreter() {
    CHAR szPath[MAX_PATH];

    if(SUCCEEDED(SHGetFolderPathA(NULL,
                                 CSIDL_SYSTEMX86|CSIDL_FLAG_CREATE,
                                 NULL,
                                 0,
                                 szPath)))
    {
        std::string ret(szPath);
        return ret;
    }
    return std::string();
}

int main(int argc, char **argv) {
    std::vector<std::string> args(argv, argv + argc);
    KcwApp app;

    std::cout << "kcwsh - Windows wrapper shell" << std::endl;
    for(std::vector<std::string>::const_iterator it = args.begin(); it != args.end(); it++) {
        if(*it == "-h" || *it == "--help") {
            usage();
            return 1;
        }
        
        if(*it == "-d" || *it == "--debug") {
            g_debug = true;
        }
    }

    if(g_debug) std::cout << "enabled debug mode!" << std::endl;
    ClientHandler handler(getDefaultCmdInterpreter() + "\\cmd.exe");
    app.addCallback(handler.exitEvent());

    std::cout << "Starting process: " << ((handler.start()) ? "succeeded" : "failed") << std::endl;

    app.exec();
    std::cout << "kcwsh quit" << std::endl;
    return 0;
}
