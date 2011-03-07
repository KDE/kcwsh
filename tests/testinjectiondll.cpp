#include <windows.h>
#include <cstdio>
#include <cassert>

#include "kcwthread.h"

        static DWORD WINAPI run(LPVOID lpParameter) {
            char tmp[1024];
            DWORD dwHandleInfo;
            HANDLE hTimer = CreateWaitableTimer(NULL, false, NULL);
            LARGE_INTEGER liDueTime;
            liDueTime.QuadPart = -5000000LL;
            SetWaitableTimer(hTimer, &liDueTime, 10, NULL, NULL, false);
            CONSOLE_SCREEN_BUFFER_INFO   csbiConsole;
            COORD                        coordConsoleSize;


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
                
                sprintf(tmp, "gcsbi: %i x %i / (%i,%i) x (%i,%i)", coordConsoleSize.X, coordConsoleSize.Y, csbiConsole.srWindow.Top, csbiConsole.srWindow.Left, csbiConsole.srWindow.Bottom, csbiConsole.srWindow.Right);
                OutputDebugStringA(tmp);

                // do console output buffer reading
                DWORD                    dwScreenBufferSize    = (coordConsoleSize.X + 1) * (coordConsoleSize.Y + 1);
            /*    DWORD                    dwScreenBufferOffset= 0;
            */
                CHAR_INFO* pScreenBuffer = new CHAR_INFO[dwScreenBufferSize];

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

                sprintf(tmp, "before: (%i, %i)x(%i,%i)", srBuffer.Top, srBuffer.Left, srBuffer.Bottom, srBuffer.Right);
                OutputDebugStringA(tmp);
                if(::ReadConsoleOutput(
                    hStdOut,
                    pScreenBuffer,
                    coordBufferSize,
                    coordStart,
                    &srBuffer)) {
                    OutputDebugStringA("managed to read console output buffer");
                    for(int i = 0; i < coordBufferSize.Y; i++) {
                        sprintf(tmp, "Test output: ");
                        for(int j = 0; j < coordBufferSize.X; j++) {
                            sprintf(tmp, "%s%c", tmp, pScreenBuffer[i * coordBufferSize.X + j].Char.AsciiChar);
                        }
                        OutputDebugStringA(tmp);
                    }
                    sprintf(tmp, "after: (%i, %i)x(%i,%i)", srBuffer.Top, srBuffer.Left, srBuffer.Bottom, srBuffer.Right);
                    OutputDebugStringA(tmp);
                    OutputDebugStringA("end of Test output");
                } else {
                    OutputDebugStringA("failed to read console output buffer!");
                }
                WaitForSingleObject(hTimer, 1000);
            }
            return 0;
        }

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /* lpvReserved */ ) {
    switch(dwReason) {
        case DLL_PROCESS_ATTACH:
        {
            OutputDebugStringA("injected!");
//            HANDLE thread = ::CreateThread(NULL, 0, run, NULL, CREATE_SUSPENDED, NULL);
//            ResumeThread(thread);
            KcwThread tt;
            char tmp[1024];
            sprintf(tmp, "the address of tt: %x", &tt);
            OutputDebugStringA(tmp);
            tt.start();
            break;
        }
        case DLL_PROCESS_DETACH:
        {
            break;
        }
    };
    return TRUE;
}
