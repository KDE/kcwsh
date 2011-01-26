#include "clienthandler.h"

ClientHandler::ClientHandler( std::string procname ) : m_procName( procname ) {
}

std::string GetModulePath(HMODULE hModule)
{
    TCHAR szModulePath[MAX_PATH+1];
    ::ZeroMemory(szModulePath, (MAX_PATH+1));

    ::GetModuleFileName(hModule, szModulePath, MAX_PATH);

    std::string strPath(szModulePath);

    return strPath.substr(0, strPath.rfind('\\'));
}

bool ClientHandler::start(HANDLE _stdin, HANDLE _stdout, HANDLE _stderr) {
    STARTUPINFO siWow;
    ::ZeroMemory(&siWow, sizeof(STARTUPINFO));

    siWow.cb            = sizeof(STARTUPINFO);
    if(_stdin) siWow.hStdInput = _stdin;
    if(_stdout) siWow.hStdOutput = _stdout;
    if(_stderr) siWow.hStdError = _stderr;
//    siWow.dwFlags       = STARTF_USESHOWWINDOW;
//    siWow.wShowWindow   = SW_HIDE;
    

    if (!::CreateProcess(
            NULL,
            const_cast<TCHAR*>(m_procName.c_str()),
            NULL,
            NULL,
            FALSE,
            0,
            NULL,
            NULL,
            &siWow,
            &m_procInfo))
    {
        return false;
    }
    return true;
}

bool ClientHandler::inject() {
    // allocate memory for parameter in the remote process
    std::string      strHookDllPath(GetModulePath(NULL));

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

    DWORD dwExitCode = WaitForSingleObject(m_procInfo.hProcess, INFINITE); 
    return true;
}
