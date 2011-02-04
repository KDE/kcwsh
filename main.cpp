#include <iostream>
#include <string>
#include <vector>

#include <windows.h>
#include <Shlobj.h>

#include "clienthandler.h"
#include "pipehandler.h"
#include "kcwapp.h"

#define BUFSIZE 4096

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

/*DWORD ReadFromPipe(HANDLE childStdIn, HANDLE waitHandle) 

// Read output from the child process's pipe for STDOUT
// and write to the parent process's pipe for STDOUT.
// Stop when there is no more data.
{
   DWORD dwRead, dwWritten, ret = 0;
   CHAR chBuf[BUFSIZE];
   char tmp[1024];
   BOOL bSuccess = FALSE;
   HANDLE hParentStdIn      = GetStdHandle(STD_INPUT_HANDLE);

    for (;;)
    {
        bSuccess = ReadFile(hParentStdIn, chBuf, BUFSIZE, &dwRead, NULL);
        sprintf(tmp, "read string: ");
        for(unsigned i = 0; i < dwRead; i++) {
            sprintf(tmp, "%s %i", tmp, chBuf[i]);
            if(chBuf[i] == 0x1b) {
                OutputDebugString("found an escape sequence!");
            }
        }
        OutputDebugString(tmp);
        if(!bSuccess) {
            cout << "no success reading from stdin" << endl;
            break;
        }

        bSuccess = WriteFile(childStdIn, chBuf,
                           dwRead, &dwWritten, NULL);
        if(!bSuccess) {
            cout << "no success writing stdin to client" << endl;
            break;
        }

        ret = ::WaitForSingleObject(waitHandle, 10);
        if(ret == WAIT_OBJECT_0) {
            break;
        }

        if(ret == WAIT_ABANDONED || ret == WAIT_FAILED) {
            cout << "process died in an unusual way!" << endl;
            break;
        }
    }
    return ret;
}*/

static HANDLE childStdIn = NULL;
static HANDLE childStdOut = NULL;

void transferStdIn() {
    DWORD dwRead, dwWritten, ret = 0;
    CHAR chBuf[BUFSIZE];
    char tmp[1024];
    BOOL bSuccess = FALSE;
    HANDLE hParentStdIn = GetStdHandle(STD_INPUT_HANDLE);

    bSuccess = ReadFile(hParentStdIn, chBuf, BUFSIZE, &dwRead, NULL);
    sprintf(tmp, "read string: ");
    for(unsigned i = 0; i < dwRead; i++) {
        sprintf(tmp, "%s %i", tmp, chBuf[i]);
        if(chBuf[i] == 0x1b) {
            OutputDebugString("found an escape sequence!");
        }
    }
    OutputDebugString(tmp);
    if(!bSuccess) {
        cout << "no success reading from stdin" << endl;
        return;
    }

    bSuccess = WriteFile(childStdIn, chBuf,
                       dwRead, &dwWritten, NULL);
    if(!bSuccess) {
        cout << "no success writing stdin to client" << endl;
        return;
    }
}

void transferStdOut() {
    DWORD dwRead, dwWritten, ret = 0;
    CHAR chBuf[BUFSIZE];
    char tmp[1024];
    BOOL bSuccess = FALSE;
    HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    bSuccess = ReadFile(childStdOut, chBuf, BUFSIZE, &dwRead, NULL);
    sprintf(tmp, "read string: ");
    for(unsigned i = 0; i < dwRead; i++) {
        sprintf(tmp, "%s %i", tmp, chBuf[i]);
        if(chBuf[i] == 0x1b) {
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
}

int main(int argc, char **argv) {
    vector<string> args(argv, argv + argc);
    KcwApp app;

    cout << "kcwsh - Windows wrapper shell" << endl;
    for(vector<string>::const_iterator it = args.begin(); it != args.end(); it++) {
        if(*it == "-h" || *it == "--help") {
            usage();
            return 1;
        }
    }

    ClientHandler handler(getDefaultCmdInterpreter() + "\\cmd.exe");
    PipeHandler in(PipeHandler::STDIN_PIPE);
//    PipeHandler out(PipeHandler::STDOUT_PIPE);
/*    PipeHandler err(PipeHandler::STDERR_PIPE);*/
    
    cout << "Starting process: " << ((handler.start(in.readHandle(), GetStdHandle(STD_OUTPUT_HANDLE), GetStdHandle(STD_ERROR_HANDLE))) ? "succeeded" : "failed") << endl;
    handler.createMonitor();
    childStdIn = in.writeHandle();
    app.addCallback(GetStdHandle(STD_INPUT_HANDLE), &transferStdIn);
//    app.addCallback(out.readHandle(), &transferStdOut);
    app.addCallback(handler.childProcess());
    
    app.exec();
    cout << "kcwsh quit" << endl;
    return 0;
}