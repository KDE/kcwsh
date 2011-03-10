#include <windows.h>
#include <cstdio>
#include <cassert>

#include "kcwthread.h"

class MyThread : public KcwThread {
    public:
        DWORD run() {
        WCHAR tmp[1024];
        HANDLE hTimer = CreateWaitableTimer(NULL, false, NULL);
        LARGE_INTEGER liDueTime;
        liDueTime.QuadPart = -5000000LL;
        SetWaitableTimer(hTimer, &liDueTime, 10, NULL, NULL, false);
        CONSOLE_SCREEN_BUFFER_INFO  csbiConsole;
        COORD                       coordConsoleSize;
        
        COORD                       coordCursor;
        COORD                       coordCurrentCursor;

        OutputDebugString(L"starting thread");
        CHAR_INFO* pCurrentScreenBuffer = 0;
        CHAR_INFO* pScreenBuffer = 0;
        for(;;) {
            HANDLE hStdOut = ::CreateFileA("CONOUT$",
                                        GENERIC_WRITE | GENERIC_READ,
                                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                                        NULL,
                                        OPEN_EXISTING,
                                        0,
                                        0);
            ::GetConsoleScreenBufferInfo(hStdOut, &csbiConsole);

            coordConsoleSize.X    = csbiConsole.srWindow.Right - csbiConsole.srWindow.Left + 1;
            coordConsoleSize.Y    = csbiConsole.srWindow.Bottom - csbiConsole.srWindow.Top + 1;
            coordCursor = csbiConsole.dwCursorPosition;
            
//            wsprintf(tmp, L"gcsbi: %i x %i / (%i,%i) x (%i,%i)", coordConsoleSize.X, coordConsoleSize.Y, csbiConsole.srWindow.Top, csbiConsole.srWindow.Left, csbiConsole.srWindow.Bottom, csbiConsole.srWindow.Right);
//            OutputDebugString(tmp);

            // do console output buffer reading
            DWORD                    dwScreenBufferSize    = (coordConsoleSize.X + 1) * (coordConsoleSize.Y + 1);
        /*    DWORD                    dwScreenBufferOffset= 0;
        */
            if(pScreenBuffer == 0) pScreenBuffer = new CHAR_INFO[dwScreenBufferSize];
            if(pCurrentScreenBuffer == 0) pCurrentScreenBuffer = new CHAR_INFO[dwScreenBufferSize];
            
            bool startup = true;

            COORD        coordBufferSize;
            // start coordinates for the buffer are always (0, 0) - we use offset
            COORD        coordStart = {0, 0};
            SMALL_RECT    srBuffer;

        //    TRACE(L"===================================================================\n");

            // ReadConsoleOutput seems to fail for large (around 6k CHAR_INFO's) buffers
            // here we calculate max buffer size (row count) for safe reading
            coordBufferSize.X    = csbiConsole.srWindow.Right - csbiConsole.srWindow.Left + 1;
        //    coordBufferSize.Y    = 6144 / coordBufferSize.X;
            coordBufferSize.Y    = 20;

            // initialize reading rectangle
            srBuffer.Top        = csbiConsole.srWindow.Bottom - coordBufferSize.Y + 1;
            srBuffer.Bottom        = csbiConsole.srWindow.Bottom;
            srBuffer.Left        = csbiConsole.srWindow.Left;
            srBuffer.Right        = csbiConsole.srWindow.Right;
    //                srBuffer.Right        = csbiConsole.srWindow.Left + csbiConsole.srWindow.Right - csbiConsole.srWindow.Left;

            wsprintf(tmp, L"before: (%i, %i)x(%i,%i)", srBuffer.Top, srBuffer.Left, srBuffer.Bottom, srBuffer.Right);
            if(::ReadConsoleOutput(
                hStdOut,
                pScreenBuffer,
                coordBufferSize,
                coordStart,
                &srBuffer)) {
                bool textChanged = (::memcmp(pCurrentScreenBuffer, pScreenBuffer, dwScreenBufferSize*sizeof(CHAR_INFO)) != 0);
                if(textChanged) {
                    OutputDebugString(tmp);
                    for(int i = 0; i < coordBufferSize.Y; i++) {
                        wsprintf(tmp, L"Test output: ");
                        for(int j = 0; j < coordBufferSize.X; j++) {
                            wsprintf(tmp, L"%s%c", tmp, pScreenBuffer[i * coordBufferSize.X + j].Char.AsciiChar);
                        }
                        OutputDebugString(tmp);
                    }
                    wsprintf(tmp, L"after: (%i, %i)x(%i,%i)", srBuffer.Top, srBuffer.Left, srBuffer.Bottom, srBuffer.Right);
                    OutputDebugString(tmp);
                    OutputDebugString(L"end of Test output");
                    ::CopyMemory(pCurrentScreenBuffer, pScreenBuffer, dwScreenBufferSize*sizeof(CHAR_INFO));
                    bool textChanged = (::memcmp(pCurrentScreenBuffer, pScreenBuffer, dwScreenBufferSize*sizeof(CHAR_INFO)) != 0);
                }
            } else {
                OutputDebugString(L"failed to read console output buffer!");
            }
            coordCurrentCursor = coordCursor;
            WaitForSingleObject(hTimer, 100);
        }
        delete[] pScreenBuffer;
        delete[] pCurrentScreenBuffer;
        return 0;
    }
};

static MyThread tt;
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /* lpvReserved */ ) {
    switch(dwReason) {
        case DLL_PROCESS_ATTACH:
        {
            OutputDebugString(L"injected!");
            WCHAR tmp[1024];
            tt.start();
            wsprintf(tmp, L"the address of tt: %x", &tt);
            OutputDebugString(tmp);
            break;
        }
        case DLL_PROCESS_DETACH:
        {
            break;
        }
    };
    return TRUE;
}
