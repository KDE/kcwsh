#include <iostream>
#include <string>
#include <vector>

#include <windows.h>
#include <Shlobj.h>

#include <kcwapp.h>
#include <kcwdebug.h>

#include "consoleterminal.h"

#define BUFSIZE 4096

static bool g_debug = false;

using namespace KcwSH;

void usage() {
    std::cout << "kcwsh is a wrapper Shell for cmd.exe which understands ansi " << std::endl \
         << "escape sequences for some functionality." << std::endl \
         << std::endl << "options:" << std::endl \
         << "-h, --help" << "\tDisplays this help message." << std::endl \
         << "-d, --debug" << "\tswitches on debug mode" << std::endl \
         << std::endl << "Copyright (C) by Patrick Spendrin 2011" << std::endl \
         << "Redistributable under GPLv3" << std::endl;
}

std::wstring getDefaultCmdInterpreter() {
    WCHAR szPath[MAX_PATH];

    if(SUCCEEDED(SHGetFolderPathW(NULL,
                                 CSIDL_SYSTEM|CSIDL_FLAG_CREATE,
                                 NULL,
                                 0,
                                 szPath)))
    {
        std::wstring ret(szPath);
        return ret;
    }
    return std::wstring();
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
    ConsoleTerminal term;
    term.setCmd(getDefaultCmdInterpreter() + L"\\cmd.exe");
    term.start();
    app.addCallback(term.exitEvent());
    app.exec();
    KcwDebug() << "kcwsh quit";
    std::cout << "kcwsh quit" << std::endl;
    return 0;
}
