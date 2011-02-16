#include <cstdio>
#include <windows.h>

#include "kcwsharedmemory.h"
#include "serverhandler.h"

static ServerHandler srv;

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /* lpvReserved */ ) {
    switch(dwReason) {
        case DLL_PROCESS_ATTACH:
        {
            srv.connect();
            break;
        }
        case DLL_PROCESS_DETACH:

            srv.disconnect();
            break;
    };
    return TRUE;
}
