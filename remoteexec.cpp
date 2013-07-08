#include "remoteexec.h"
#include <kcwdebug.h>

#include <cctype>

//KcwSharedMemory<COORD> RemoteExec::s_bufferSize;
//KcwSharedMemory<CHAR_INFO> RemoteExec::s_buffer;
KcwNotifier RemoteExec::s_exitEvent;
InputWriter RemoteExec::s_inputWriter;
KcwSharedMemory<HWND> RemoteExec::s_consoleWindow;

static HANDLE myThreadHandle = NULL;

__declspec(dllexport) void startMyThread() {
    ResumeThread(myThreadHandle);
}

extern "C" __declspec(dllexport) DWORD WINAPI myThreadProc(LPVOID lpParameter) {
    KcwDebug() << "calling myThreadProc";
//    reinterpret_cast<InputWriter*>(lpParameter)->exec();
    return 0;
}

RemoteExec::RemoteExec() {
}

// open the shared memories
void RemoteExec::openConnections() {
    std::wstringstream wss;
    WCHAR tmp[1024];
    ZeroMemory(tmp, 1024);
    DWORD dwProcessId = ::GetCurrentProcessId();

    KcwDebug() << "opening connections";

/*    wsprintf(tmp, L"kcwsh-bufferSize-%x", dwProcessId);
    if(s_bufferSize.open(tmp) != 0) {
        KcwDebug() << "failed to open bufferSize";
        s_bufferSize.errorExit();
    };

    wsprintf(tmp, L"kcwsh-buffer-%x", dwProcessId);
    if(s_buffer.open(tmp) != 0) {
        KcwDebug() << "failed to open buffer";
        s_buffer.errorExit();
    };
*/
    wsprintf(tmp, L"kcwsh-exitEvent-%i", dwProcessId);
    KcwDebug() << "trying to open exitEvent:" << (const wchar_t*)tmp;
    if(s_exitEvent.open(tmp) != 0) {
        KcwDebug() << "failed to open exitEvent:" << (const wchar_t*)tmp;
    };

/*    wsprintf(tmp, L"kcwsh-consoleWindow-%i", dwProcessId);
    if(s_consoleWindow.open(tmp) != 0) {
        KcwDebug() << "failed to open consoleWindow:" << (const wchar_t*)tmp;
//        s_contentCheck.errorExit();
    };*/
    s_inputWriter.init();
    
//     HANDLE myThread = CreateThread(NULL, 0, myThreadProc, &s_inputWriter, CREATE_SUSPENDED, NULL);
    //s_inputWriter.exec();

    HWND hwnd = GetConsoleWindow();
    *s_consoleWindow = hwnd;

/*    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);

    DWORD fdwMode, oldMode;
    GetConsoleMode(hStdin, &oldMode);
    fdwMode = oldMode;
    fdwMode ^= ENABLE_LINE_INPUT;
    fdwMode ^= ENABLE_MOUSE_INPUT;
    SetConsoleMode(hStdin, fdwMode);
    INPUT_RECORD recs[10];
    ZeroMemory(recs, sizeof(INPUT_RECORD)*10);
    int num, cnt = 0;
    while(cnt < 0) {
        ReadConsoleInput(hStdin, recs, 1, (DWORD*)&num);
        if(recs[0].EventType != KEY_EVENT) continue;
        if(recs[0].Event.KeyEvent.bKeyDown) {
            KcwDebug() << (char)recs[0].Event.KeyEvent.uChar.AsciiChar
                       << (int)recs[0].Event.KeyEvent.wRepeatCount
                       << (int)recs[0].Event.KeyEvent.wVirtualKeyCode
                       << (int)recs[0].Event.KeyEvent.wVirtualScanCode;
        }
        cnt += num;
    }*/
}

// all the static callback functions
/*void RemoteExec::bufferSizeCallback(void *obj) {
    BOOL bSuccess = FALSE;
    HANDLE hStdOut;
    COORD   finalCoordBufferSize;

    KcwDebug() << "resizing request for" << s_bufferSize->X << "x" << s_bufferSize->Y;

    hStdOut = ::CreateFileA( "CONOUT$",
                            GENERIC_WRITE | GENERIC_READ,
                            FILE_SHARE_READ | FILE_SHARE_WRITE,
                            NULL,
                            OPEN_EXISTING,
                            0,
                            0);

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    ::GetConsoleScreenBufferInfo(hStdOut, &csbi);
    KcwDebug() << "Console size:" << csbi.dwSize.X << "x" << csbi.dwSize.Y << endl;

    // first, resize rows
    finalCoordBufferSize.X  = s_bufferSize->X;
    finalCoordBufferSize.Y  = csbi.dwSize.Y; // for now, allow resizing only in X direction (width)

    // if new buffer size is > than old one, we need to resize the buffer first
    ::SetConsoleScreenBufferSize(hStdOut, finalCoordBufferSize);
}
*/

void RemoteExec::bufferContentCheck(void *obj) {
/*    DWORD dwHandleInfo;
    static CHAR_INFO* pCurrentScreenBuffer = 0;
    HANDLE hStdOut = ::CreateFileA("CONOUT$",
                                GENERIC_WRITE | GENERIC_READ,
                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL,
                                OPEN_EXISTING,
                                0,
                                0);


    CONSOLE_SCREEN_BUFFER_INFO   csbiConsole;
    COORD                        coordConsoleSize;

    ::GetConsoleScreenBufferInfo(hStdOut, &csbiConsole);

    coordConsoleSize.X    = csbiConsole.srWindow.Right - csbiConsole.srWindow.Left + 1;
    coordConsoleSize.Y    = csbiConsole.srWindow.Bottom - csbiConsole.srWindow.Top + 1;

    KcwDebug() << "gcsbi:" << coordConsoleSize.X << "x" << coordConsoleSize.Y 
                           << "/ (" << csbiConsole.srWindow.Left << "," << csbiConsole.srWindow.Top
                           << ") x (" << csbiConsole.srWindow.Right << "," << csbiConsole.srWindow.Bottom
                           << ")";

    // do console output buffer reading
    DWORD                    dwScreenBufferSize    = coordConsoleSize.X * coordConsoleSize.Y;
//    DWORD                    dwScreenBufferOffset= 0;

    CHAR_INFO* pScreenBuffer = new CHAR_INFO[dwScreenBufferSize];
    if(pCurrentScreenBuffer == 0) pCurrentScreenBuffer = new CHAR_INFO[dwScreenBufferSize];

    COORD        coordBufferSize;
    // start coordinates for the buffer are always (0, 0) - we use offset
    COORD        coordStart = {0, 0};
    SMALL_RECT    srBuffer;

//    TRACE(L"===================================================================\n");

    // ReadConsoleOutput seems to fail for large (around 6k CHAR_INFO's) buffers
    // here we calculate max buffer size (row count) for safe reading
    coordBufferSize.X    = csbiConsole.srWindow.Right - csbiConsole.srWindow.Left + 1;
    coordBufferSize.X    = 10;
//    coordBufferSize.Y    = 6144 / coordBufferSize.X;
    coordBufferSize.Y    = 11;

    // initialize reading rectangle
    srBuffer.Top        = csbiConsole.srWindow.Bottom - coordBufferSize.Y;
    srBuffer.Bottom        = csbiConsole.srWindow.Bottom + 2;
    srBuffer.Left        = csbiConsole.srWindow.Left;
    srBuffer.Right        = csbiConsole.srWindow.Left + csbiConsole.srWindow.Right - csbiConsole.srWindow.Left;

    // read rows 'chunks'
//    SHORT i = 0;
//    for (; i < coordConsoleSize.Y / coordBufferSize.Y; ++i)
//    {
//        TRACE(L"Reading region: (%i, %i) - (%i, %i)\n", srBuffer.Left, srBuffer.Top, srBuffer.Right, srBuffer.Bottom);

    KcwDebug() << "before: (" << srBuffer.Top << "," << srBuffer.Left << ") x (" << srBuffer.Bottom << "," << srBuffer.Right << ")";
    if(!GetHandleInformation(hStdOut, &dwHandleInfo)) {
        KcwDebug() << "stdout handle is broken!";
    }
    if(::ReadConsoleOutput(
        hStdOut,
        pScreenBuffer,
        coordBufferSize,
        coordStart,
        &srBuffer)) {
        KcwDebug() << "managed to read console output buffer";
        for(int i = 0; i < coordBufferSize.Y; i++) {
            KcwDebug() << "Test output: ";
            for(int j = 0; j < coordBufferSize.X; j++) {
//                sprintf(tmp, "%s%c", tmp, pScreenBuffer[i * coordBufferSize.X + j].Char.AsciiChar);
            }
//            OutputDebugStringA(tmp);
        }
        bool textChanged = (::memcmp(pCurrentScreenBuffer, pScreenBuffer, dwScreenBufferSize*sizeof(CHAR_INFO)) != 0);
        if(textChanged) {
            ::CopyMemory(pCurrentScreenBuffer, pScreenBuffer, dwScreenBufferSize*sizeof(CHAR_INFO));
            ::CopyMemory(s_buffer.data(), pScreenBuffer, dwScreenBufferSize*sizeof(CHAR_INFO));
            s_buffer.notify();
        }
        KcwDebug() << "after: (" << srBuffer.Top << "," << srBuffer.Left << ") x (" << srBuffer.Bottom << "," << srBuffer.Right << ")";
        KcwDebug() << "end of Test output";
    } else {
        KcwDebug() << "failed to read console output buffer!";
    }*/

//        srBuffer.Top        = srBuffer.Top + coordBufferSize.Y;
//        srBuffer.Bottom        = srBuffer.Bottom + coordBufferSize.Y;

//        dwScreenBufferOffset += coordBufferSize.X * coordBufferSize.Y;
//    }

    // read the last 'chunk', we need to calculate the number of rows in the
    // last chunk and update bottom coordinate for the region
//    coordBufferSize.Y    = coordConsoleSize.Y - i * coordBufferSize.Y;
//    srBuffer.Bottom        = csbiConsole.srWindow.Bottom;

//    ::ReadConsoleOutput(
//        hStdOut.get(),
//        pScreenBuffer.get() + dwScreenBufferOffset,
//        coordBufferSize,
//        coordStart,
//        &srBuffer);


    // compare previous buffer, and if different notify Console
//    SharedMemoryLock consoleInfoLock(m_consoleInfo);
//    SharedMemoryLock bufferLock(m_consoleBuffer);

//    bool textChanged = (::memcmp(m_consoleBuffer.Get(), pScreenBuffer.get(), m_dwScreenBufferSize*sizeof(CHAR_INFO)) != 0);

//    if ((::memcmp(&m_consoleInfo->csbi, &csbiConsole, sizeof(CONSOLE_SCREEN_BUFFER_INFO)) != 0) ||
//        (m_dwScreenBufferSize != dwScreenBufferSize) ||
//        textChanged)
//    static std::string buffercontents;
}
/*
// getters for the notification events
HANDLE RemoteExec::bufferSizeNotification() {
    return s_bufferSize.notificationEvent();
}
*/
HANDLE RemoteExec::contentNotification() {
//    return s_contentCheck.notificationEvent();
    return NULL;
}

HANDLE RemoteExec::exitEvent() {
    return s_exitEvent.handle();
}
