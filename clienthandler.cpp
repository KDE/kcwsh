#include <iostream>

#include "clienthandler.h"

ClientHandler::ClientHandler( std::string procname )
    : m_procName( procname ) {

}

ClientHandler::~ClientHandler() {
    quit();
    std::cout << "exiting clienthandler!" << std::endl;
    OutputDebugString("exiting clienthandler!");
}

HANDLE ClientHandler::childProcess() {
    return m_procInfo.hProcess;
}

std::string ClientHandler::getModulePath(HMODULE hModule) {
    TCHAR szModulePath[MAX_PATH+1];
    ::ZeroMemory(szModulePath, (MAX_PATH+1));

    ::GetModuleFileName(hModule, szModulePath, MAX_PATH);

    std::string strPath(szModulePath);

    return strPath.substr(0, strPath.rfind('\\'));
}

bool ClientHandler::start(HANDLE _stdin, HANDLE _stdout, HANDLE _stderr) {
    STARTUPINFO siWow;
    char tmp[1024];

    ::ZeroMemory(&siWow, sizeof(STARTUPINFO));

    siWow.cb            = sizeof(STARTUPINFO);
    if(_stdin) siWow.hStdInput = _stdin;
    if(_stdout) siWow.hStdOutput = _stdout;
    if(_stderr) siWow.hStdError = _stderr;
    siWow.dwFlags       = STARTF_USESTDHANDLES;
    DWORD dwStartupFlags = CREATE_SUSPENDED;
//	| DETACHED_PROCESS; // the detached process won't work
//    siWow.dwFlags       |= STARTF_USESHOWWINDOW;
//    siWow.wShowWindow   = SW_HIDE;

    if (!::CreateProcess(
            NULL,
            const_cast<TCHAR*>(m_procName.c_str()),
            NULL,
            NULL,
            TRUE,
            dwStartupFlags,
            NULL,
            NULL,
            &siWow,
            &m_procInfo))
    {
		OutputDebugString("failed to create process!");
        return false;
    }
    sprintf(tmp, "kcwsh-exitEvent-%x", m_procInfo.dwProcessId);
    if(m_sharedExitEvent.create(std::string(tmp), 1) != 0) {
        m_sharedExitEvent.errorExit();
    };

    sprintf(tmp, "kcwsh-contentCheck-%x", m_procInfo.dwProcessId);
    if(m_contentCheck.create(std::string(tmp), 1) != 0) {
        m_contentCheck.errorExit();
    };

    // before we can use the handle in the client process, we need to duplicate it
    ::DuplicateHandle(  GetCurrentProcess(),
                        exitEvent(),
                        m_procInfo.hProcess,
                        &(*m_sharedExitEvent),
                        0,
                        FALSE,
                        DUPLICATE_SAME_ACCESS);

    addCallback(m_procInfo.hProcess);

    if (!inject()) {
		OutputDebugString("failed to inject dll!");
		return false;
	}

    setRefreshInterval(10);
    KcwThread::start();
    ::ResumeThread(m_procInfo.hThread);
	// test notification stuff:
	m_contentCheck.notify();
    return true;
}

DWORD ClientHandler::processId() const {
    return m_procInfo.dwProcessId;
}
HANDLE ClientHandler::contentCheckNotifyEvent() {
	return m_contentCheck.notificationEvent();
}

bool ClientHandler::inject() {
    // allocate memory for parameter in the remote process
    std::string      strHookDllPath(getModulePath(NULL));

    CONTEXT     context;

    void*       mem             = NULL;
    size_t      memLen          = 0;
    UINT_PTR    fnLoadLibrary   = NULL;

    size_t      codeSize = 20;

    strHookDllPath += std::string("\\kcwshhook.dll");

    ::ZeroMemory(&context, sizeof(CONTEXT));

    BYTE* code = new BYTE[codeSize + (MAX_PATH)];

    memLen = (strHookDllPath.length() + 1);

    CopyMemory(code + codeSize, strHookDllPath.c_str(), memLen);
    memLen += codeSize;

    context.ContextFlags = CONTEXT_FULL;
    GetThreadContext(m_procInfo.hThread, &context);

    mem = VirtualAllocEx(m_procInfo.hProcess, NULL, memLen, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    fnLoadLibrary = (UINT_PTR)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");

    union
    {
        PBYTE  pB;
        PINT   pI;
        PULONGLONG pL;
    } ip;

    ip.pB = code;

    *ip.pB++ = 0x68;            // push  eip
    *ip.pI++ = context.Eip;
    *ip.pB++ = 0x9c;            // pushf
    *ip.pB++ = 0x60;            // pusha
    *ip.pB++ = 0x68;            // push  "path\to\our.dll"
    *ip.pI++ = (UINT_PTR)mem + codeSize;
    *ip.pB++ = 0xe8;            // call  LoadLibraryW
    *ip.pI++ = (UINT_PTR)fnLoadLibrary - ((UINT_PTR)mem + (ip.pB + 4 - code));
    *ip.pB++ = 0x61;            // popa
    *ip.pB++ = 0x9d;            // popf
    *ip.pB++ = 0xc3;            // ret

    WriteProcessMemory(m_procInfo.hProcess, mem, code, memLen, NULL);
    FlushInstructionCache(m_procInfo.hProcess, mem, memLen);
    context.Eip = (UINT_PTR)mem;
    SetThreadContext(m_procInfo.hThread, &context);
    return true;
}
