#include <cstdio>
#include <windows.h>

#include "sharedmemory.h"

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /* lpvReserved */ ) {
    switch(dwReason) {
        case DLL_PROCESS_ATTACH:
        {
            char tmp[1024];
            DWORD dwProcessId = ::GetCurrentProcessId();
            DWORD dwWritten;
            HANDLE hStdin, hStdout;
            BOOL bSuccess;
            SharedMemory<int> m_test;
            
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
            sprintf(tmp, "kcwsh-%x", dwProcessId);
            m_test.open(tmp);
            
            sprintf(tmp, "running in process with id: %i %i", dwProcessId, *m_test);
            OutputDebugString(tmp);
            break;
        }
    };
    return TRUE;
}
