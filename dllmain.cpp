#include <windows.h>

#include <kcwdebug.h>
#include <kcwnotifier.h>
#include <string>

HINSTANCE s_module = 0;
KcwNotifier s_notifier;

extern "C" __declspec(dllexport) BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /* lpvReserved */ ) {
    s_module = hInstance;
    switch(dwReason) {
        case DLL_PROCESS_ATTACH:
        {
            std::wstringstream wss;
            wss << L"kcwsh-exitEvent-" << GetCurrentProcessId();
            s_notifier.open(wss.str());
            break;
        }
        case DLL_PROCESS_DETACH:
        {
            KcwDebug() << "detaching from process!" << ::GetCurrentProcessId();
            s_notifier.notify();
            break;
        }
        case DLL_THREAD_ATTACH:
        {
            KcwDebug() << "attaching thread!";
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
