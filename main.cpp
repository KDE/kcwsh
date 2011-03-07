#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include <windows.h>
#include <Shlobj.h>

#include "clienthandler.h"
#include "pipehandler.h"
#include "kcwsharedmemory.h"
#include "kcwapp.h"

#define BUFSIZE 4096

static bool g_debug = false;
//static HANDLE childStdIn = NULL;
//static HANDLE childStdOut = NULL;
//static HANDLE parentStdIn = GetStdHandle(STD_INPUT_HANDLE);
//static bool first_run = false;

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

/*void transferStdOut() {
    DWORD dwRead, dwWritten, ret = 0;
    CHAR chBuf[BUFSIZE];
    char tmp[1024];
    BOOL bSuccess = FALSE;
    HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    FlushConsoleInputBuffer(parentStdIn);

    bSuccess = ReadFile(childStdOut, chBuf, BUFSIZE, &dwRead, NULL);
    sprintf(tmp, "read string: ");
    for(unsigned i = 0; i < dwRead; i++) {
        sprintf(tmp, "%s %i", tmp, chBuf[i]);
        std::cout << "test:" << (chBuf[i] == '\\') << (chBuf[i + 1] == '\\') << (chBuf[i + 2] == 'e') << std::endl;
        if(chBuf[i] == 0x1b || (g_debug && chBuf[i] == '\\' && chBuf[i + 1] == '\\' && chBuf[i + 2] == 'e')) {
            if(g_debug) std::cout << "got escape sequence!" << std::endl;
            OutputDebugStringA("found an escape sequence!");
        }
    }
    
    // the following line would result in to much output, and it only stops
    // if you enter stuff :-/
    // OutputDebugStringA(tmp);
    if(dwRead > 0) OutputDebugStringA(tmp);
    if(!bSuccess) {
//        std::cout << "no success reading from childs stdout" << std::endl;
        return;
    }

    bSuccess = WriteFile(hParentStdOut, chBuf,
                       dwRead, &dwWritten, NULL);
    if(!bSuccess) {
        std::cout << "no success writing stdout to our stdout" << std::endl;
        return;
    }
}*/

int main(int argc, char **argv) {
    std::vector<std::string> args(argv, argv + argc);
    KcwApp app;
    char tmp[1024];

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
    InputPipe in;
//    PipeHandler out(PipeHandler::STDOUT_PIPE);
/*    PipeHandler err(PipeHandler::STDERR_PIPE);*/
    app.addCallback(in.exitEvent());
    app.addCallback(handler.exitEvent());
    in.start();

//    std::cout << "Starting process: " << ((handler.start(GetStdHandle(STD_INPUT_HANDLE), GetStdHandle(STD_OUTPUT_HANDLE), GetStdHandle(STD_ERROR_HANDLE))) ? "succeeded" : "failed") << std::endl;
    std::cout << "Starting process: " << ((handler.start(in.readHandle(), GetStdHandle(STD_OUTPUT_HANDLE), GetStdHandle(STD_ERROR_HANDLE))) ? "succeeded" : "failed") << std::endl;
	in.setContentCheckEvent(handler.contentCheckNotifyEvent());
//    childStdIn = in.writeHandle();

//    app.addCallback(parentStdIn, &transferStdIn);
//    app.addCallback(out.readHandle(), &transferStdOut);
    
    app.exec();
    std::cout << "kcwsh quit" << std::endl;
    return 0;
}
