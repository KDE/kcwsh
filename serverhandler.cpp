#include "serverhandler.h"
#include <windows.h>

ServerHandler::ServerHandler() {
}

void ServerHandler::connect() {
    char tmp[1024];
    DWORD dwProcessId = ::GetCurrentProcessId();
    sprintf(tmp, "kcwsh-%x", dwProcessId);
    m_test.open(tmp);
    
    sprintf(tmp, "running in process with id: %i %i", dwProcessId, *m_test);
    OutputDebugString(tmp);
}

void ServerHandler::disconnect() {
}