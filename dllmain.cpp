#include <cstdio>
#include <windows.h>

#include "sharedmemory.h"
#include "serverhandler.h"

static ServerHandler srv;

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /* lpvReserved */ ) {
    switch(dwReason) {
        case DLL_PROCESS_ATTACH:
        {
            DWORD dwWritten;
            HANDLE hStdin, hStdout;
            BOOL bSuccess;
            
            srv.connect();
            hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
            hStdin = GetStdHandle(STD_INPUT_HANDLE);
            if( hStdout ) {
                bSuccess = WriteFile(hStdout, "bla\n", 3, &dwWritten, NULL);
            } else {
                LPVOID lpMsgBuf;
                DWORD dw = GetLastError(); 

                FormatMessage(
                    FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,
                    dw,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    (LPTSTR) &lpMsgBuf,
                    0, NULL );
                OutputDebugString((LPCTSTR)lpMsgBuf);
            }
            break;
        }
        case DLL_PROCESS_DETACH:

            srv.disconnect();
            break;
    };
    return TRUE;
}
