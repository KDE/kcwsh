#include "kcwsharedmemory.h"
#include "kcwdebug.h"

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /* lpvReserved */ ) {
    switch(dwReason) {
        case DLL_PROCESS_ATTACH:
        {
            KcwSharedMemory<int> shmem(L"injectortest", 1, false);
            *shmem += 1;
            break;
        }
        case DLL_PROCESS_DETACH:
        {
            break;
        }
    };
    return TRUE;
}
