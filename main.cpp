#include <iostream>
#include <string>
#include <vector>

#include <windows.h>
#include <Shlobj.h>

#include "clienthandler.h"
#include "pipehandler.h"

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

DWORD ReadFromPipe(HANDLE childStdIn, HANDLE waitHandle) 

// Read output from the child process's pipe for STDOUT
// and write to the parent process's pipe for STDOUT.
// Stop when there is no more data.
{
   DWORD dwRead, dwWritten, ret = 0;
   CHAR chBuf[BUFSIZE];
   BOOL bSuccess = FALSE;
   HANDLE hParentStdIn      = GetStdHandle(STD_INPUT_HANDLE);

    for (;;)
    {
        bSuccess = ReadFile(hParentStdIn, chBuf, BUFSIZE, &dwRead, NULL);
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

    ClientHandler handler(getDefaultCmdInterpreter() + "\\cmd.exe");
    PipeHandler in(PipeHandler::STDIN_PIPE);
/*    PipeHandler out(PipeHandler::STDOUT_PIPE);
    PipeHandler err(PipeHandler::STDERR_PIPE);*/
    
    cout << "Starting process: " << ((handler.start(in.readHandle(), GetStdHandle(STD_OUTPUT_HANDLE), GetStdHandle(STD_OUTPUT_HANDLE))) ? "succeeded" : "failed") << endl;
    handler.createMonitor();
    ReadFromPipe(in.writeHandle(), handler.childProcess());
    return 0;
}