#include "terminal.h"

#include <kcwinjector.h>
#include <kcwdebug.h>
#include <kcweventloop.h>

#include <windows.h>

#include "hooks.h"

extern HINSTANCE s_module;

using namespace KcwSH;

Terminal::Terminal()
: m_process("cmd.exe")
, KcwThread() {
}

Terminal::~Terminal() {
}

std::wstring Terminal::getModulePath(HMODULE hModule) {
    WCHAR szModulePath[MAX_PATH + 1];
    ::ZeroMemory(szModulePath, (MAX_PATH + 1)*sizeof(WCHAR));

    ::GetModuleFileName(hModule, szModulePath, MAX_PATH);

    std::wstring strPath(szModulePath);

    return strPath.substr(0, strPath.rfind('\\'));
}

void Terminal::setCmd(const std::string& _cmd) {
    m_process.setCmd(_cmd);
}

std::string Terminal::cmd() const {
    return m_process.cmd();
}

DWORD Terminal::run() {
    // 1) create a shell process in suspended mode (default)
    m_process.start();
    addCallback(m_process.process());

    m_inputReader.setProcess(m_process.pid());
    m_inputReader.init(GetStdHandle(STD_INPUT_HANDLE));
    m_inputReader.start();

/*    std::wstringstream wss;
    wss << "kcwsh-exitEvent-" << m_process.pid();
    KcwDebug() << "creating exitEvent: " << wss.str();
    if(m_exitEvent.open(wss.str().c_str()) != 0) {
        KcwDebug() << "could not create exitEvent";
        return -1;
    };*/

    // 2) inject this very dll into that process
    KcwInjector injector;
    injector.setInjectionDll(getModulePath(NULL) + L"\\kcwsh.dll");
    injector.setDestinationProcess(m_process.process(), m_process.thread());

    if (!injector.inject()) {
        KcwDebug() << "failed to inject dll!";
        return -2;
    }

    // 3) create a remote thread which does the handling of input
    LPTHREAD_START_ROUTINE pfnThreadRoutine = (LPTHREAD_START_ROUTINE)((char*)injector.baseAddress() + ((char*)kcwshInputHook - (char*)s_module));
    HANDLE hRemoteThread = CreateRemoteThread(m_process.process(), NULL, 0, pfnThreadRoutine, NULL, 0, NULL);
//    addCallback(hRemoteThread);

    m_process.resume();
    return KcwEventLoop::exec();
}
