#include <cstdio>
#include <windows.h>

#include "remoteexec.h"

//static ServerHandler srv;

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /* lpvReserved */ ) {
    switch(dwReason) {
        case DLL_PROCESS_ATTACH:
        {
            RemoteExec::openConnections();
//            srv.connect();
            break;
        }
        case DLL_PROCESS_DETACH:
        {
//            srv.disconnect();
            break;
        }
    };
    return TRUE;
}
