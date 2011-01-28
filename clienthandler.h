#ifndef clienthandler_h
#define clienthandler_h

#include <string>

#include <windows.h>

#include "sharedmemory.h"

class ClientHandler {
    public:
        ClientHandler(std::string procname);
        ~ClientHandler();
        
        bool start(HANDLE _stdin = 0, HANDLE _stdout = 0, HANDLE _stderr = 0);
        bool inject();
        bool stop();

        DWORD createMonitor();
        void cleanMonitor();
        HANDLE childProcess();

    private:
        static DWORD WINAPI monitorThreadStatic(LPVOID lpParameter);
        DWORD monitor();
        std::string getModulePath(HMODULE hModule);

        PROCESS_INFORMATION m_procInfo;
        std::string m_procName;
        SharedMemory<int> m_test;

        HANDLE m_monitorThreadExitEvent;
        HANDLE m_monitorThread;
};

#endif /* clienthandler_h */
