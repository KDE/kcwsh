#include "clienthandler.h"

#include <iostream>
#include <windows.h>

#include <kcwdebug.h>

#include "inputreader.h"

ClientHandler::ClientHandler( std::string procname )
    : m_process( procname ) {
    m_injector.setInjectionDll(getModulePath(NULL) + L"\\kcwshhook.dll");
}

ClientHandler::~ClientHandler() {
    m_process.quit();
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

    m_process.start();
    m_injector.setDestinationProcess(m_process.process(), m_process.thread());

    // initialize forward thread
    m_input.setProcess(m_process.pid());
    HANDLE _stdin = GetStdHandle(STD_INPUT_HANDLE);

    wss << "kcwsh-exitEvent-" << m_process.pid();
    KcwDebug() << "creating exitEvent: " << wss.str();
    if(m_sharedExitEvent.open(wss.str().c_str()) != 0) {
        KcwDebug() << "could not create sharedExitEvent";
        return false;
    };

    m_input.init(_stdin);

/*    wss.str(L"");
    wss << "kcwsh-contentCheck-" << m_process.pid();
//     KcwDebug() << "creating contentCheck:" << wss.str();
    if(m_contentCheck.create(wss.str().c_str()) != 0) {
        KcwDebug() << "could not create contentCheck";
        m_contentCheck.errorExit();
    };
*/
    addCallback(m_process.process());

    if (!m_injector.inject()) {
        KcwDebug() << "failed to inject dll!";
        return false;
    }
    KcwDebug() << "injector injected!";
    m_input.start();
    KcwThread::start();
    HMODULE hThisModule = LoadLibraryW((getModulePath(NULL) + L"\\kcwshhook.dll").c_str());
    FARPROC localMethodAddr = GetProcAddress(hThisModule, "_myThreadProc@4");
    DWORD methodDelta = (DWORD)localMethodAddr - (DWORD)hThisModule;
    DWORD remoteMethodAddr = m_injector.baseAddress() + methodDelta;
    LPVOID ProcessBaseAdress = VirtualAllocEx ( HanProcess, NULL, sizeof ( ChaDLLFilePath ), MEM_COMMIT, PAGE_READWRITE );
    m_process.resume();
//    HANDLE myRemoteThread = CreateRemoteThread(m_process.process(), NULL, 0, (LPTHREAD_START_ROUTINE)remoteMethodAddr, 0, NULL, NULL);
    KcwDebug() << "thread started!";
    return true;
}