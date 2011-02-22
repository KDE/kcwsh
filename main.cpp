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

using namespace std;

static bool g_debug = false;
static HANDLE childStdIn = NULL;
static HANDLE childStdOut = NULL;
static HANDLE parentStdIn = GetStdHandle(STD_INPUT_HANDLE);
static bool first_run = false;
static KcwSharedMemory<int> bufferSize;

void usage() {
    cout << "kcwsh is a wrapper Shell for cmd.exe which understands ansi " << endl \
         << "escape sequences for some functionality." << endl \
         << endl << "options:" << endl \
         << "-h, --help" << "\tDisplays this help message." << endl \
         << "-d, --debug" << "\tswitches on debug mode" << endl \
         << endl << "Copyright (C) by Patrick Spendrin 2011" << endl \
         << "Redistributable under GPLv3" << endl;
}

std::string getDefaultCmdInterpreter() {
    TCHAR szPath[MAX_PATH];

    if(SUCCEEDED(SHGetFolderPath(NULL,
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
        cout << "test:" << (chBuf[i] == '\\') << (chBuf[i + 1] == '\\') << (chBuf[i + 2] == 'e') << endl;
        if(chBuf[i] == 0x1b || (g_debug && chBuf[i] == '\\' && chBuf[i + 1] == '\\' && chBuf[i + 2] == 'e')) {
            if(g_debug) cout << "got escape sequence!" << endl;
            OutputDebugString("found an escape sequence!");
        }
    }
    
    // the following line would result in to much output, and it only stops
    // if you enter stuff :-/
    // OutputDebugString(tmp);
    if(dwRead > 0) OutputDebugString(tmp);
    if(!bSuccess) {
//        cout << "no success reading from childs stdout" << endl;
        return;
    }

    bSuccess = WriteFile(hParentStdOut, chBuf,
                       dwRead, &dwWritten, NULL);
    if(!bSuccess) {
        cout << "no success writing stdout to our stdout" << endl;
        return;
    }
}*/

int main(int argc, char **argv) {
    vector<string> args(argv, argv + argc);
    KcwApp app;
    DWORD test = 0;
    if( GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &test) ) {
        char tmp[1024];
        sprintf(tmp, "console mode: %i (%i, %i)", test, (test & ENABLE_ECHO_INPUT), (test & ~ENABLE_LINE_INPUT));
        OutputDebugString(tmp);
        SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), (test & ~ENABLE_LINE_INPUT));
    } else {
        OutputDebugString("couldn't set console mode");
    }

    cout << "kcwsh - Windows wrapper shell" << endl;
    for(vector<string>::const_iterator it = args.begin(); it != args.end(); it++) {
        if(*it == "-h" || *it == "--help") {
            usage();
            return 1;
        }
        
        if(*it == "-d" || *it == "--debug") {
            g_debug = true;
        }
    }

    if(g_debug) cout << "enabled debug mode!" << endl;
    ClientHandler handler(getDefaultCmdInterpreter() + "\\cmd.exe");
    InputPipe in;
//    PipeHandler out(PipeHandler::STDOUT_PIPE);
/*    PipeHandler err(PipeHandler::STDERR_PIPE);*/
    in.start();

    cout << "Starting process: " << ((handler.start(in.readHandle(), GetStdHandle(STD_OUTPUT_HANDLE), GetStdHandle(STD_ERROR_HANDLE))) ? "succeeded" : "failed") << endl;
	in.setContentCheckEvent(handler.contentCheckNotifyEvent());
    char tmp[1024];
    OutputDebugString("trying to open");
    sprintf(tmp, "kcwsh-bufferSize-%x", handler.processId());
    if(bufferSize.create(tmp, 2) != 0) {
        bufferSize.errorExit();
    };
    OutputDebugString(tmp);
    OutputDebugString("opened!");
//    childStdIn = in.writeHandle();

    app.addCallback(in.exitEvent());
    app.addCallback(handler.exitEvent());
//    app.addCallback(parentStdIn, &transferStdIn);
//    app.addCallback(out.readHandle(), &transferStdOut);
    
    app.exec();
    cout << "kcwsh quit" << endl;
    return 0;
}
