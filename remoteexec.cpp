#include "remoteexec.h"

KcwSharedMemory<int> RemoteExec::s_bufferSize;
KcwSharedMemory<HANDLE> RemoteExec::s_exitEvent;
KcwSharedMemory<HANDLE> RemoteExec::s_contentCheck;

RemoteExec::RemoteExec() {
}

// open the shared memories
void RemoteExec::openConnections() {
    char tmp[1024];
    DWORD dwProcessId = ::GetCurrentProcessId();

    sprintf(tmp, "kcwsh-bufferSize-%x", dwProcessId);
    if(s_bufferSize.open(tmp) != 0) {
        s_bufferSize.errorExit();
    };

    sprintf(tmp, "kcwsh-exitEvent-%x", dwProcessId);
    if(s_exitEvent.open(std::string(tmp)) != 0) {
        s_exitEvent.errorExit();
    };

    sprintf(tmp, "kcwsh-contentCheck-%x", dwProcessId);
    if(s_contentCheck.open(std::string(tmp)) != 0) {
        s_contentCheck.errorExit();
    };

}

// all the static callback functions
void RemoteExec::bufferSizeCallback(void *obj) {
    char tmp[1024], chBuf[1024];
    DWORD dwRead;
    BOOL bSuccess = FALSE;
    HANDLE hStdOut;
    COORD   finalCoordBufferSize;

    sprintf(tmp, "resizing request for %i x %i", s_bufferSize[0], s_bufferSize[1]);
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
    finalCoordBufferSize.X  = s_bufferSize[0];
    finalCoordBufferSize.Y  = csbi.dwSize.Y; // for now, allow resizing only in X direction (width)

    // if new buffer size is > than old one, we need to resize the buffer first
    ::SetConsoleScreenBufferSize(hStdOut, finalCoordBufferSize);
}

void RemoteExec::bufferContentCheck(void *obj) {
	char tmp[1024];
	HANDLE hStdOut = ::CreateFile("CONOUT$",
								GENERIC_WRITE | GENERIC_READ,
								FILE_SHARE_READ | FILE_SHARE_WRITE,
								NULL,
								OPEN_EXISTING,
								0,
								0);

	CONSOLE_SCREEN_BUFFER_INFO	csbiConsole;
	COORD						coordConsoleSize;

	::GetConsoleScreenBufferInfo(hStdOut, &csbiConsole);

	coordConsoleSize.X	= csbiConsole.srWindow.Right - csbiConsole.srWindow.Left + 1;
	coordConsoleSize.Y	= csbiConsole.srWindow.Bottom - csbiConsole.srWindow.Top + 1;
	
	sprintf(tmp, "gcsbi: %i x %i / (%i,%i) x (%i,%i)", coordConsoleSize.X, coordConsoleSize.Y, csbiConsole.srWindow.Left, csbiConsole.srWindow.Top, csbiConsole.srWindow.Right, csbiConsole.srWindow.Bottom);
	OutputDebugString(tmp);

	// do console output buffer reading
	DWORD					dwScreenBufferSize	= coordConsoleSize.X * coordConsoleSize.Y;
/*	DWORD					dwScreenBufferOffset= 0;
*/
	CHAR_INFO* pScreenBuffer = new CHAR_INFO[dwScreenBufferSize];

	COORD		coordBufferSize;
	// start coordinates for the buffer are always (0, 0) - we use offset
	COORD		coordStart = {0, 0};
	SMALL_RECT	srBuffer;

//	TRACE(L"===================================================================\n");

	// ReadConsoleOutput seems to fail for large (around 6k CHAR_INFO's) buffers
	// here we calculate max buffer size (row count) for safe reading
	coordBufferSize.X	= csbiConsole.srWindow.Right - csbiConsole.srWindow.Left + 1;
//	coordBufferSize.Y	= 6144 / coordBufferSize.X;
	coordBufferSize.Y	= 10;

	// initialize reading rectangle
	srBuffer.Top		= csbiConsole.srWindow.Bottom - coordBufferSize.Y;
	srBuffer.Bottom		= csbiConsole.srWindow.Bottom + 1;
	srBuffer.Left		= csbiConsole.srWindow.Left;
	srBuffer.Right		= csbiConsole.srWindow.Left + csbiConsole.srWindow.Right - csbiConsole.srWindow.Left;

	// read rows 'chunks'
//	SHORT i = 0;
//	for (; i < coordConsoleSize.Y / coordBufferSize.Y; ++i)
//	{
//		TRACE(L"Reading region: (%i, %i) - (%i, %i)\n", srBuffer.Left, srBuffer.Top, srBuffer.Right, srBuffer.Bottom);

	if(::ReadConsoleOutput(
		hStdOut,
		pScreenBuffer,
		coordBufferSize,
		coordStart,
		&srBuffer)) {
		OutputDebugString("managed to read console output buffer");
		for(int i = 0; i < coordBufferSize.Y; i++) {
			sprintf(tmp, "Test output: ");
			for(int j = 0; j < coordBufferSize.X; j++) {
				sprintf(tmp, "%s%c", tmp, pScreenBuffer[i * coordBufferSize.X + j].Char.AsciiChar);
			}
			OutputDebugString(tmp);
		}
		OutputDebugString("end of Test output");
	} else {
		OutputDebugString("failed to read console output buffer!");
	}

//		srBuffer.Top		= srBuffer.Top + coordBufferSize.Y;
//		srBuffer.Bottom		= srBuffer.Bottom + coordBufferSize.Y;

//		dwScreenBufferOffset += coordBufferSize.X * coordBufferSize.Y;
//	}

	// read the last 'chunk', we need to calculate the number of rows in the
	// last chunk and update bottom coordinate for the region
//	coordBufferSize.Y	= coordConsoleSize.Y - i * coordBufferSize.Y;
//	srBuffer.Bottom		= csbiConsole.srWindow.Bottom;

//	::ReadConsoleOutput(
//		hStdOut.get(), 
//		pScreenBuffer.get() + dwScreenBufferOffset, 
//		coordBufferSize, 
//		coordStart, 
//		&srBuffer);


	// compare previous buffer, and if different notify Console
//	SharedMemoryLock consoleInfoLock(m_consoleInfo);
//	SharedMemoryLock bufferLock(m_consoleBuffer);

/*	bool textChanged = (::memcmp(m_consoleBuffer.Get(), pScreenBuffer.get(), m_dwScreenBufferSize*sizeof(CHAR_INFO)) != 0);

	if ((::memcmp(&m_consoleInfo->csbi, &csbiConsole, sizeof(CONSOLE_SCREEN_BUFFER_INFO)) != 0) ||
		(m_dwScreenBufferSize != dwScreenBufferSize) ||
		textChanged)*/
//    static std::string buffercontents;
}

// getters for the notification events
HANDLE RemoteExec::bufferSizeNotification() {
    return s_bufferSize.notificationEvent();
}

HANDLE RemoteExec::contentNotification() {
    return s_contentCheck.notificationEvent();
}

HANDLE RemoteExec::exitEvent() {
    return *s_exitEvent;
}
