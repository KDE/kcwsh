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
		CONSOLE_SCREEN_BUFFER_INFO   csbiConsole;
		COORD                        coordConsoleSize;

		OutputDebugString(L"starting thread");
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
			
			wsprintf(tmp, L"gcsbi: %i x %i / (%i,%i) x (%i,%i)", coordConsoleSize.X, coordConsoleSize.Y, csbiConsole.srWindow.Top, csbiConsole.srWindow.Left, csbiConsole.srWindow.Bottom, csbiConsole.srWindow.Right);
			OutputDebugString(tmp);

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

			wsprintf(tmp, L"before: (%i, %i)x(%i,%i)", srBuffer.Top, srBuffer.Left, srBuffer.Bottom, srBuffer.Right);
			OutputDebugString(tmp);
			if(::ReadConsoleOutput(
				hStdOut,
				pScreenBuffer,
				coordBufferSize,
				coordStart,
				&srBuffer)) {
				OutputDebugString(L"managed to read console output buffer");
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
			} else {
				OutputDebugString(L"failed to read console output buffer!");
			}
			WaitForSingleObject(hTimer, 1000);
		}
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
