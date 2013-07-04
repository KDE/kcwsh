#ifndef clienthandler_h
#define clienthandler_h

#include <string>

#include <windows.h>

#include "kcwsharedmemory.h"
#include "kcwthread.h"
#include "kcwinjector.h"
#include "kcwprocess.h"

#include "pipehandler.h"

class ClientHandler : public KcwThread {
    public:
        ClientHandler(std::string procname);
        ~ClientHandler();

        bool start();

    private:
        std::wstring getModulePath(HMODULE hModule);
        KcwInjector m_injector;
        KcwProcess m_process;

        InputPipe m_inputPipe;
        OutputPipe m_outputPipe;

        KcwSharedMemory<HANDLE> m_sharedExitEvent;
        KcwSharedMemory<HANDLE> m_contentCheck;
};

#endif /* clienthandler_h */
