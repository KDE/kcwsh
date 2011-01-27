#include <iostream>

#include "clienthandler.h"

ClientHandler::ClientHandler( std::string procname )
    : m_procName( procname ),
      m_monitorThreadExitEvent( ::CreateEvent(NULL, FALSE, FALSE, NULL) ) {
}

ClientHandler::~ClientHandler() {
    std::cout << "exiting clienthandler!" << std::endl;
    OutputDebugString("exiting clienthandler!");
    cleanMonitor();
}

DWORD WINAPI ClientHandler::monitorThreadStatic(LPVOID lpParameter) {
    ClientHandler* ch = reinterpret_cast<ClientHandler*>(lpParameter);
    return ch->monitor();
}

DWORD ClientHandler::monitor() {
    HANDLE arrWaitHandles[] = { m_procInfo.hProcess, m_monitorThreadExitEvent };
    while (::WaitForMultipleObjects(sizeof(arrWaitHandles)/sizeof(arrWaitHandles[0]), arrWaitHandles, FALSE, INFINITE) > WAIT_OBJECT_0 + 1)
    {
    }
    return 0;
}

HANDLE ClientHandler::childProcess() {
    return m_procInfo.hProcess;
}

bool ClientHandler::stop() {
    ::SetEvent(m_monitorThreadExitEvent);
    ::WaitForSingleObject(m_monitorThread, 1000);
    return true;
}

std::string ClientHandler::getModulePath(HMODULE hModule) {
    TCHAR szModulePath[MAX_PATH+1];
    ::ZeroMemory(szModulePath, (MAX_PATH+1));

    ::GetModuleFileName(hModule, szModulePath, MAX_PATH);

    std::string strPath(szModulePath);

    return strPath.substr(0, strPath.rfind('\\'));
}

DWORD ClientHandler::createMonitor() {
    DWORD dwThreadId = 0;
    m_monitorThread = ::CreateThread( NULL,
                                      0, 
                                      monitorThreadStatic, 
                                      reinterpret_cast<void*>(this), 
                                      0, 
                                      &dwThreadId
                                    );
    return dwThreadId;
}

void ClientHandler::cleanMonitor() {
    ::WaitForSingleObject(m_monitorThread, 10000);
    ::CloseHandle(m_monitorThread);
}

bool ClientHandler::start(HANDLE _stdin, HANDLE _stdout, HANDLE _stderr) {
    STARTUPINFO siWow;
    ::ZeroMemory(&siWow, sizeof(STARTUPINFO));

    siWow.cb            = sizeof(STARTUPINFO);
    if(_stdin) siWow.hStdInput = _stdin;
    if(_stdout) siWow.hStdOutput = _stdout;
    if(_stderr) siWow.hStdError = _stderr;
    siWow.dwFlags       = STARTF_USESTDHANDLES;
    DWORD dwStartupFlags = CREATE_SUSPENDED;
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
        return false;
    }

	if (!inject()) return false;

	::ResumeThread(m_procInfo.hThread);
    return true;
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

//    DWORD dwExitCode = WaitForSingleObject(m_procInfo.hProcess, INFINITE); 
    return true;
}
