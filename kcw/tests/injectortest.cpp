#include "kcwprocess.h"
#include "kcwapp.h"
#include "kcwinjector.h"
#include "kcwsharedmemory.h"

#define KcwTestAssert(argument, output) if(!(argument)) { OutputDebugString(output); exit(1); }

std::wstring getModulePath(HMODULE hModule) {
    WCHAR szModulePath[MAX_PATH + 1];
    ::ZeroMemory(szModulePath, (MAX_PATH + 1)*sizeof(WCHAR));

    ::GetModuleFileName(hModule, szModulePath, MAX_PATH);

    std::wstring strPath(szModulePath);

    return strPath.substr(0, strPath.rfind('\\'));
}


int main(int argc, char**argv) {
    KcwApp app;
    KcwProcess proc("cmd.exe");
    KcwInjector injector, injector2;
    KcwSharedMemory<int> shmem;
    HANDLE timer = CreateWaitableTimer(NULL, FALSE, NULL);
    int retval;
    
    injector2.setInjectionDll(getModulePath(NULL) + L"\\commoninjector.dll");
    KcwTestAssert((injector2.inject() == false), L"KcwInjector didn't fail when no process was set");

    if(!proc.start()) {
        printf("process failed to start!");
        return -1;
    }

    shmem.create(L"injectortest");
    *shmem = 1;
    injector.setDestinationProcess(proc.process(), proc.thread());
    injector.setInjectionDll(getModulePath(NULL) + L"\\commoninjector.dll");
    if(!injector.inject()) {
        printf("failed to inject dll");
        return -1;
    }

    app.addCallback(proc.process());
    LARGE_INTEGER li;
    li.QuadPart = -20 * 1000000LL; // 2 seconds
    SetWaitableTimer(timer, &li, 0, NULL, NULL, FALSE);
    app.addCallback(timer, CB(KcwProcess::quit), &proc);
    proc.resume();
    retval = app.exec();
    KcwTestAssert((*shmem == 2), L"injected dll didn't run");
    
    return retval;
}