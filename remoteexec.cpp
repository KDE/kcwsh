#include "remoteexec.h"

KcwSharedMemory<int> RemoteExec::m_bufferSize;

RemoteExec::RemoteExec() {
}

// open the conn
void RemoteExec::openConnections() {
    char tmp[1024];
    DWORD dwProcessId = ::GetCurrentProcessId();
    sprintf(tmp, "kcwsh-bufferSize-%x", dwProcessId);
    m_bufferSize.open(tmp);
}

// all the static callback functions
void RemoteExec::bufferSizeCallback() {
    char tmp[1024], chBuf[1024];
    DWORD dwRead;
    BOOL bSuccess = FALSE;
    HANDLE hStdOut;
    COORD   finalCoordBufferSize;

    sprintf(tmp, "resizing request for %i x %i", m_bufferSize[0], m_bufferSize[1]);
    OutputDebugString(tmp);

    hStdOut = ::CreateFile( "CONOUT$",
                            GENERIC_WRITE | GENERIC_READ,
                            FILE_SHARE_READ | FILE_SHARE_WRITE,
                            NULL,
                            OPEN_EXISTING,
                            0,
                            0);

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    ::GetConsoleScreenBufferInfo(hStdOut, &csbi);
    sprintf(tmp, "Console size: %ix%i\n", csbi.dwSize.X, csbi.dwSize.Y);
    OutputDebugString(tmp);

    // first, resize rows
    finalCoordBufferSize.X  = m_bufferSize[0];
    finalCoordBufferSize.Y  = csbi.dwSize.Y; // for now, allow resizing only in X direction (width)

    // if new buffer size is > than old one, we need to resize the buffer first
    ::SetConsoleScreenBufferSize(hStdOut, finalCoordBufferSize);
}

// getters for the notification events
HANDLE RemoteExec::bufferSizeNotification() {
    return m_bufferSize.notificationEvent();
}
