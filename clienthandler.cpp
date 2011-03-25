#include "clienthandler.h"

#include <iostream>

#include "kcwdebug.h"


ClientHandler::ClientHandler( std::string procname )
    : m_process( procname ) {
    m_injector.setInjectionDll(getModulePath(NULL) + L"\\kcwshhook.dll");
}

ClientHandler::~ClientHandler() {
    quit();
    KcwDebug() << "exiting clienthandler!";
}

std::wstring ClientHandler::getModulePath(HMODULE hModule) {
    WCHAR szModulePath[MAX_PATH + 1];
    ::ZeroMemory(szModulePath, (MAX_PATH + 1)*sizeof(WCHAR));

    ::GetModuleFileName(hModule, szModulePath, MAX_PATH);

    std::wstring strPath(szModulePath);

    return strPath.substr(0, strPath.rfind('\\'));
}

bool ClientHandler::start() {
    std::wstringstream wss;
//    m_process.setStdHandle(_stdin, KcwProcess::KCW_STDIN_HANDLE);
//    m_process.setStdHandle(_stdout, KcwProcess::KCW_STDOUT_HANDLE);
//    m_process.setStdHandle(_stderr, KcwProcess::KCW_STDERR_HANDLE);
    m_process.start();

    wss << "kcwsh-exitEvent-" << m_process.pid() << std::endl;
    if(m_sharedExitEvent.create(wss.str().c_str()) != 0) {
        m_sharedExitEvent.errorExit();
    };

    wss.flush();
    wss << "kcwsh-contentCheck-" << m_process.pid();
    KcwDebug() << wss.str();
    if(m_contentCheck.create(wss.str().c_str()) != 0) {
        m_contentCheck.errorExit();
    };

    // before we can use the handle in the client process, we need to duplicate it
    ::DuplicateHandle(  GetCurrentProcess(),
                        exitEvent(),
                        m_process.process(),
                        &(*m_sharedExitEvent),
                        0,
                        FALSE,
                        DUPLICATE_SAME_ACCESS);

    addCallback(m_process.process());

    addCallback(m_process.process(), m_outputPipe.exitEvent());
    m_outputPipe.start();
    KcwDebug() << "outputpipe started!";
    if (!m_injector.inject()) {
        KcwDebug() << "failed to inject dll!";
        return false;
    }
    KcwThread::start();
    m_process.resume();
    KcwDebug() << "injector injected!";
    return true;
}