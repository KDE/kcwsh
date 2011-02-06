#include <windows.h>
#include "serverhandler.h"
#include "remoteexec.h"

HANDLE g_stdIn;
HANDLE g_stdOut;

ServerHandler::ServerHandler()
  : KcwEventLoop(::CreateEvent(NULL, FALSE, FALSE, "KcwServerHandler")), 
    m_thread(NULL) {
    g_stdOut = ::CreateFile(
                        "CONOUT$",
                        GENERIC_WRITE | GENERIC_READ,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        NULL,
                        OPEN_EXISTING,
                        0,
                        0);

    g_stdIn = ::CreateFile(
                        "CONIN$",
                        GENERIC_WRITE | GENERIC_READ,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        NULL,
                        OPEN_EXISTING,
                        0,
                        0);
}

void ServerHandler::connect() {
    char tmp[1024];
    DWORD dwProcessId = ::GetCurrentProcessId();
    DWORD dwThreadId = 0;
    
    sprintf(tmp, "running in process with id: %i", dwProcessId);
    OutputDebugString(tmp);

    sprintf(tmp, "kcwsh-sharedExitEvent-%x", dwProcessId);
    m_sharedExitEvent.open(std::string(tmp));

    RemoteExec::openConnections();
    addCallback(RemoteExec::bufferSizeNotification(), &RemoteExec::bufferSizeCallback);
    m_thread = ::CreateThread(NULL, 0, monitorThreadStatic, reinterpret_cast<void*>(this), 0, &dwThreadId);
}

void ServerHandler::disconnect() {
    // quit the eventloop
    quit();
    ::SetEvent(*m_sharedExitEvent);
    ::WaitForSingleObject(m_thread, 10000);
    OutputDebugString("disconnected!");
}

DWORD WINAPI ServerHandler::monitorThreadStatic(LPVOID lpParameter) {
    ServerHandler* pServerHandler = reinterpret_cast<ServerHandler*>(lpParameter);
    return pServerHandler->exec();
}
