#include <windows.h>
#include "serverhandler.h"
#include "remoteexec.h"
#include "kcw/kcwdebug.h"

ServerHandler::ServerHandler() {
}

void ServerHandler::connect() {
    RemoteExec::openConnections();

    // this must be handled special: when calling KcwEventLoop::quit(), you also signal
    // to the other side that now the application has to quit.
    setExitEvent(RemoteExec::exitEvent());

    // add application local callbacks
    addCallback(RemoteExec::bufferSizeNotification(), &RemoteExec::bufferSizeCallback, this);
    addCallback(RemoteExec::contentNotification(), &RemoteExec::bufferContentCheck, this);
    KcwThread::start();
}

void ServerHandler::disconnect() {
    // quit the eventloop and thus also the thread
    quit();
    KcwDebug() << "disconnected!";
}
