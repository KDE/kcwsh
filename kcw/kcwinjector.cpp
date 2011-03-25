#include "kcwinjector.h"
#include "kcwdebug.h"

KcwInjector::KcwInjector()
 : m_destProcess(NULL),
   m_destThread(NULL) {
}

void KcwInjector::setDestinationProcess(HANDLE proc, HANDLE thread) {
    m_destProcess = proc;
    m_destThread = thread;
}

void KcwInjector::setInjectionDll(std::wstring dllPath) {
    m_dllPath = dllPath;
}

bool KcwInjector::inject() {
    CONTEXT     context;

    void*       mem             = NULL;
    size_t      memLen          = 0;
    UINT_PTR    fnLoadLibrary   = NULL;

    size_t      codeSize = 20;

    ::ZeroMemory(&context, sizeof(CONTEXT));

    if(m_destProcess == NULL || m_destThread == NULL) {
        KcwDebug() << "destination process or thread are empty: process"
                   << m_destProcess << "thread" << m_destThread;
        return false;
    }

    if (::GetFileAttributes(m_dllPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
        KcwDebug() << "invalid file attributes for file" << m_dllPath.c_str();
        return false;
    }

    BYTE* code = new BYTE[codeSize + (m_dllPath.length() + 1) * sizeof(wchar_t)];

    memLen = (m_dllPath.length() + 1) * sizeof(wchar_t);
    CopyMemory(code + codeSize, m_dllPath.c_str(), memLen);
    memLen += codeSize;

    context.ContextFlags = CONTEXT_FULL;
    GetThreadContext(m_destThread, &context);

    mem = VirtualAllocEx(m_destProcess, NULL, memLen, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    fnLoadLibrary = (UINT_PTR)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "LoadLibraryW");

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
