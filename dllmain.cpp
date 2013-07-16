#include <windows.h>

#include <kcwdebug.h>

HINSTANCE s_module = 0;

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /* lpvReserved */ ) {
    s_module = hInstance;
    switch(dwReason) {
        case DLL_PROCESS_ATTACH:
        {
            break;
        }
        case DLL_PROCESS_DETACH:
        {
            KcwDebug() << "detaching from process!";
            break;
        }
        case DLL_THREAD_DETACH:
        {
            KcwDebug() << "detaching thread!";
            break;
        }
    };
    return TRUE;
}
