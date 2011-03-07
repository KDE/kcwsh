#include "kcwinjector.h"

void KcwInjector::setDestinationProcess(HANDLE proc, HANDLE thread) {
    m_destProcess = proc;
    m_destThread = thread;
}

void KcwInjector::setInjectionDll(std::string dllPath) {
    m_dllPath = dllPath;
}

bool KcwInjector::inject() {
    CONTEXT     context;

    void*       mem             = NULL;
    size_t      memLen          = 0;
    UINT_PTR    fnLoadLibrary   = NULL;

    size_t      codeSize = 20;

    ::ZeroMemory(&context, sizeof(CONTEXT));

    BYTE* code = new BYTE[codeSize + m_dllPath.length() * sizeof(char) + 1];

    memLen = (m_dllPath.length() * sizeof(char) + 1);

    CopyMemory(code + codeSize, m_dllPath.c_str(), memLen);
    memLen += codeSize;

    context.ContextFlags = CONTEXT_FULL;
    GetThreadContext(m_destThread, &context);

    mem = VirtualAllocEx(m_destProcess, NULL, memLen, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    fnLoadLibrary = (UINT_PTR)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");

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

    WriteProcessMemory(m_destProcess, mem, code, memLen, NULL);
    FlushInstructionCache(m_destProcess, mem, memLen);
    context.Eip = (UINT_PTR)mem;
    SetThreadContext(m_destThread, &context);
    return true;
}
