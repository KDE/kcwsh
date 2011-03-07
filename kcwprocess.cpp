#include "kcwprocess.h"

KcwProcess::KcwProcess(std::string execPath)
  : m_executablePath(execPath),
    m_pid(-1),
    m_isRunning(false),
    m_procInfo(0),
    m_isStartedAsPaused(true) {
    m_stdHandles[KCW_STDIN_HANDLE] = 0;
    m_stdHandles[KCW_STDOUT_HANDLE] = 0;
    m_stdHandles[KCW_STDERR_HANDLE] = 0;
}

KcwProcess::KcwProcess(int pid)
  : m_pid(pid),
    m_isRunning(true),
    m_procInfo(0),
    m_isStartedAsPaused(true) {
    m_stdHandles[KCW_STDIN_HANDLE] = 0;
    m_stdHandles[KCW_STDOUT_HANDLE] = 0;
    m_stdHandles[KCW_STDERR_HANDLE] = 0;
}

KcwProcess::KcwProcess()
  : m_pid(-1),
    m_isRunning(false),
    m_procInfo(0),
    m_isStartedAsPaused(true) {
    m_stdHandles[KCW_STDIN_HANDLE] = 0;
    m_stdHandles[KCW_STDOUT_HANDLE] = 0;
    m_stdHandles[KCW_STDERR_HANDLE] = 0;
}

void KcwProcess::setStdHandle(HANDLE hdl, KCW_STREAM_TYPE type) {
    m_stdHandles[type] = hdl;
}

void KcwProcess::setIsStartedAsPaused(bool isPaused) {
    m_isStartedAsPaused = isPaused;
}

void KcwProcess::setExecutablePath(std::string execPath) {
    m_executablePath = execPath;
}

void KcwProcess::setStartupFlags(int stFlags) {
    m_startupFlags = stFlags;
}

bool KcwProcess::start() {
    STARTUPINFOA siWow;
    char tmp[1024];

    ::ZeroMemory(&siWow, sizeof(STARTUPINFO));

    siWow.cb            = sizeof(STARTUPINFO);
/*    if(m_stdHandles[KCW_STDIN_HANDLE]) {
        siWow.hStdInput = m_stdHandles[KCW_STDIN_HANDLE];
    } else {*/
        siWow.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
        siWow.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
        siWow.hStdError = GetStdHandle(STD_ERROR_HANDLE);
/*    }
    if(m_stdHandles[KCW_STDOUT_HANDLE]) {
        siWow.hStdOutput = m_stdHandles[KCW_STDOUT_HANDLE];
    } else {
        siWow.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    }
    if(m_stdHandles[KCW_STDERR_HANDLE]) {
        siWow.hStdError = m_stdHandles[KCW_STDERR_HANDLE];
    } else {
        siWow.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    }*/
//    siWow.dwFlags       = STARTF_USESTDHANDLES;
    DWORD dwStartupFlags = CREATE_NEW_CONSOLE|CREATE_SUSPENDED;
//    DWORD dwStartupFlags = CREATE_SUSPENDED;
//  | DETACHED_PROCESS; // the detached process won't work
//    siWow.dwFlags       |= STARTF_USESHOWWINDOW;
//    siWow.wShowWindow   = SW_HIDE;

    m_procInfo = new PROCESS_INFORMATION;
    if (!::CreateProcessA(
            NULL,
            const_cast<char*>(m_executablePath.c_str()),
            NULL,
            NULL,
            TRUE,
            dwStartupFlags,
            NULL,
            NULL,
            &siWow,
            m_procInfo))
    {
        DWORD dw = GetLastError();
        LPVOID lpMsgBuf = NULL;

        FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            dw,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR) &lpMsgBuf,
            0, NULL);

        OutputDebugStringA((LPCSTR)lpMsgBuf);
        LocalFree(lpMsgBuf);
        return false;
    }
    m_pid = m_procInfo->dwProcessId;
    return true;
}

bool KcwProcess::resume() {
    return ResumeThread(m_procInfo->hThread);
}

int KcwProcess::startupFlags() const {
    return m_startupFlags;
}

HANDLE KcwProcess::process() const {
    return m_procInfo->hProcess;
}

HANDLE KcwProcess::thread() const {
    return m_procInfo->hThread;
}

bool KcwProcess::isRunning() const {
    return m_isRunning;
}

int KcwProcess::pid() const {
    return m_pid;
}

std::string KcwProcess::executablePath() const {
    return m_executablePath;
}
