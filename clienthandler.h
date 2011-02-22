#ifndef clienthandler_h
#define clienthandler_h

#include <string>

#include <windows.h>

#include "kcwsharedmemory.h"
#include "kcwthread.h"

class ClientHandler : public KcwThread {
    public:
        ClientHandler(std::string procname);
        ~ClientHandler();

        bool start(HANDLE _stdin = 0, HANDLE _stdout = 0, HANDLE _stderr = 0);

        HANDLE childProcess();
		HANDLE contentCheckNotifyEvent();
        DWORD processId() const;

    private:
        bool inject();
        std::string getModulePath(HMODULE hModule);

        PROCESS_INFORMATION m_procInfo;
        std::string m_procName;
        KcwSharedMemory<HANDLE> m_sharedExitEvent;
        KcwSharedMemory<HANDLE> m_contentCheck;
};

#endif /* clienthandler_h */
