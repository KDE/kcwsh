#include <iostream>

#include "clienthandler.h"

ClientHandler::ClientHandler( std::string procname )
    : m_procName( procname ), m_process( procname ) {
    m_injector.setInjectionDll(getModulePath(NULL) + "\\kcwshhook.dll");
}

ClientHandler::~ClientHandler() {
    quit();
    std::cout << "exiting clienthandler!" << std::endl;
    OutputDebugStringA("exiting clienthandler!");
}

HANDLE ClientHandler::childProcess() {
    return m_process.process();
}

std::string ClientHandler::getModulePath(HMODULE hModule) {
    CHAR szModulePath[MAX_PATH + 1];
    ::ZeroMemory(szModulePath, (MAX_PATH + 1));

    ::GetModuleFileNameA(hModule, szModulePath, MAX_PATH);

    std::string strPath(szModulePath);

    return strPath.substr(0, strPath.rfind('\\'));
}

bool ClientHandler::start(HANDLE _stdin, HANDLE _stdout, HANDLE _stderr) {
//    char tmp[1024];
    m_process.setStdHandle(_stdin, KcwProcess::KCW_STDIN_HANDLE);
    m_process.setStdHandle(_stdout, KcwProcess::KCW_STDOUT_HANDLE);
    m_process.setStdHandle(_stderr, KcwProcess::KCW_STDERR_HANDLE);
    m_process.start();

//    sprintf(tmp, "kcwsh-exitEvent-%x", m_process.pid());
//    if(m_sharedExitEvent.create(std::string(tmp), 1) != 0) {
//        m_sharedExitEvent.errorExit();
//    };

//    sprintf(tmp, "kcwsh-contentCheck-%x", m_process.pid());
//    if(m_contentCheck.create(std::string(tmp), 1) != 0) {
//        m_contentCheck.errorExit();
//    };

    // before we can use the handle in the client process, we need to duplicate it
//    ::DuplicateHandle(  GetCurrentProcess(),
//                        exitEvent(),
//                        m_process.process(),
//                        &(*m_sharedExitEvent),
//                        0,
//                        FALSE,
//                        DUPLICATE_SAME_ACCESS);

    addCallback(m_process.process());

//    if (!m_injector.inject()) {
//        OutputDebugStringA("failed to inject dll!");
//        return false;
//    }

    KcwThread::start();
    m_process.resume();
    // test notification stuff:
//    m_contentCheck.notify();
    return true;
}

DWORD ClientHandler::processId() const {
    return m_process.pid();
}
HANDLE ClientHandler::contentCheckNotifyEvent() {
    return m_contentCheck.notificationEvent();
}