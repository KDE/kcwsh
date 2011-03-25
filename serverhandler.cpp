#include <windows.h>
#include "serverhandler.h"
#include "remoteexec.h"
#include "kcw/kcwdebug.h"

ServerHandler::ServerHandler() : m_timer( CreateWaitableTimer(NULL, FALSE, NULL) ) {
}

void ServerHandler::connect() {
    RemoteExec::openConnections();
    // this must be handled special: when calling KcwEventLoop::quit(), you also signal
    // to the other side that now the application has to quit.
    setExitEvent(RemoteExec::exitEvent());

    // add application local callbacks
    addCallback(RemoteExec::bufferSizeNotification(), &RemoteExec::bufferSizeCallback, this);
    addCallback(m_timer, &RemoteExec::bufferContentCheck, this);
    LARGE_INTEGER li;

    li.QuadPart = -10 * 1000000LL; // 10 milliseconds
    SetWaitableTimer(m_timer, &li, 10, NULL, NULL, FALSE);
    KcwThread::start();
}

void ServerHandler::disconnect() {
    // quit the eventloop and thus also the thread
    quit();
    KcwDebug() << "disconnected!";
}

