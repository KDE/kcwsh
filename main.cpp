#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include <windows.h>
#include <Shlobj.h>

#include "clienthandler.h"
#include "pipehandler.h"
#include "kcwapp.h"

#define BUFSIZE 4096

using namespace std;

bool g_debug = false;
static HANDLE childStdIn = NULL;
static HANDLE childStdOut = NULL;
static HANDLE parentStdIn = GetStdHandle(STD_INPUT_HANDLE);
static bool first_run = false;
static SharedMemory<int> bufferSize;

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

void parseEscapeSequence(char *esc, int length) {
    char tmp[BUFSIZE], d1;
    if(esc[0] == '[') {
        sprintf(tmp, "got escape sequence with suffix '%c' data: '%s'", esc[length - 1], &esc[1]);
        OutputDebugString(tmp);
        switch(esc[length - 1]) {
            case 't':   {   /* setting of the buffer size */
                            std::istringstream i(&esc[1]);
                            if( i >> bufferSize[0] >> d1 >> bufferSize[1] ){
                                bufferSize.notify();
                            }
                            break;
                        }
            default:    {
                        };
        };
    }
}

// this needs to be moved into its own thread, otherwise it blocks to much
void transferStdIn() {
    DWORD dwRead = 0, dwWritten, dwResult, inputEvents, ret = 0;
    CHAR chBuf[BUFSIZE];
    char tmp[1024];
    BOOL bSuccess = FALSE;
    static char buffer[BUFSIZE];
    static char bufLength = 0;
    static int beginEsc = 0;
    static bool inEscapeSeq = false;

    dwResult = WaitForSingleObject(parentStdIn, 0);
    
    if(dwResult == WAIT_OBJECT_0) {
        bSuccess = ReadFile(parentStdIn, chBuf, BUFSIZE, &dwRead, NULL);
        sprintf(tmp, "read string: ");
        for(unsigned i = 0; i < dwRead; i++) {
            buffer[bufLength + i] = chBuf[i];
            sprintf(tmp, "%s %i", tmp, chBuf[i]);
        }
        bufLength += dwRead;
        OutputDebugString(tmp);
        if(!bSuccess) {
            cout << "no success reading from stdin" << endl;
            return;
        }

        for(int j = 0; j < bufLength; j++) {
            // found the begin of an escape sequence
            if(!inEscapeSeq && (buffer[j] == 0x1b || 
               (g_debug && j > 1 && buffer[j - 2] == '\\' && buffer[j - 1] == '\\' && buffer[j] == 'e'))) {
                beginEsc = j + 1;
//                OutputDebugString("inEscapeSequence");
                inEscapeSeq = true;
            }
            
            // found the end of an escape sequence
            if(inEscapeSeq && buffer[j] == 13) {
                buffer[j] = 0;
                parseEscapeSequence(&buffer[beginEsc], j - beginEsc);
//                OutputDebugString("end inEscapeSequence");
                inEscapeSeq = false;
                if(buffer[beginEsc - 1] != 0x1b) {
                    bufLength = beginEsc - 3;
                } else {
                    bufLength = beginEsc - 1;
                }

                buffer[bufLength] = 0;
                return;
            }
        }
        
        if(!inEscapeSeq) {
            if(chBuf[0] == 13 && dwRead == 1) {
                chBuf[1] = 10;
                dwRead = 2;
            }
            bSuccess = WriteFile(childStdIn, chBuf, dwRead, &dwWritten, NULL);
            if(!bSuccess) {
                cout << "no success writing stdin to client" << endl;
                return;
            }
        }
    }
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
    PipeHandler in(PipeHandler::STDIN_PIPE);
//    PipeHandler out(PipeHandler::STDOUT_PIPE);
/*    PipeHandler err(PipeHandler::STDERR_PIPE);*/
    
    cout << "Starting process: " << ((handler.start(in.readHandle(), GetStdHandle(STD_OUTPUT_HANDLE), GetStdHandle(STD_ERROR_HANDLE))) ? "succeeded" : "failed") << endl;
    handler.createMonitor();
    char tmp[1024];
    OutputDebugString("trying to open");
    sprintf(tmp, "kcwsh-bufferSize-%x", handler.processId());
    bufferSize.create(tmp, 2);
    OutputDebugString(tmp);
    OutputDebugString("opened!");
    childStdIn = in.writeHandle();
    app.addCallback(handler.childProcess());
    app.addCallback(handler.childMonitor());
    app.addCallback(parentStdIn, &transferStdIn);
//    app.addCallback(out.readHandle(), &transferStdOut);
    
    app.exec();
    cout << "kcwsh quit" << endl;
    return 0;
}