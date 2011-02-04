#include "serverhandler.h"
#include <windows.h>

ServerHandler::ServerHandler()
  : KcwEventLoop(::CreateEvent(NULL, FALSE, FALSE, "KcwServerHandler")), 
    m_thread(NULL) {
}

void ServerHandler::connect() {
    char tmp[1024];
    DWORD dwProcessId = ::GetCurrentProcessId();
    DWORD dwThreadId = 0;
    sprintf(tmp, "kcwsh-%x", dwProcessId);
    m_test.open(tmp);
    
    sprintf(tmp, "running in process with id: %i %i", dwProcessId, *m_test);
    OutputDebugString(tmp);
    m_thread = ::CreateThread(NULL, 0, monitorThreadStatic, reinterpret_cast<void*>(this), 0, &dwThreadId);
}

void ServerHandler::disconnect() {
    // quit the eventloop
    quit();
    OutputDebugString("quitting the eventloop!");
    ::WaitForSingleObject(m_thread, 10000);
    OutputDebugString("disconnected!");
}

DWORD WINAPI ServerHandler::monitorThreadStatic(LPVOID lpParameter) {
    ServerHandler* pServerHandler = reinterpret_cast<ServerHandler*>(lpParameter);
    return pServerHandler->exec();
}
