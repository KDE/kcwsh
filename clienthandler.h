#ifndef clienthandler_h
#define clienthandler_h

#include <string>

#include <windows.h>

#include "kcwsharedmemory.h"
#include "kcwthread.h"
#include "kcwinjector.h"
#include "kcwprocess.h"

class ClientHandler : public KcwThread {
    public:
        ClientHandler(std::string procname);
        ~ClientHandler();

        bool start(HANDLE _stdin = 0, HANDLE _stdout = 0, HANDLE _stderr = 0);

        HANDLE childProcess();
		HANDLE contentCheckNotifyEvent();
        DWORD processId() const;

    private:
        std::string getModulePath(HMODULE hModule);
        KcwInjector m_injector;
        KcwProcess m_process;

        PROCESS_INFORMATION m_procInfo;
        std::string m_procName;
        KcwSharedMemory<HANDLE> m_sharedExitEvent;
        KcwSharedMemory<HANDLE> m_contentCheck;
};

#endif /* clienthandler_h */
