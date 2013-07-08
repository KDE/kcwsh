#ifndef clienthandler_h
#define clienthandler_h

#include <string>

#include <windows.h>

#include <kcwsharedmemory.h>
#include <kcwthread.h>
#include <kcwinjector.h>
#include <kcwprocess.h>
#include <kcwnotifier.h>

#include "pipehandler.h"
#include "inputreader.h"

class ClientHandler : public KcwThread {
    public:
        ClientHandler(std::string procname);
        ~ClientHandler();

        bool start();

    private:
        std::wstring getModulePath(HMODULE hModule);
        KcwInjector m_injector;
        KcwProcess m_process;

        KcwNotifier m_sharedExitEvent;
//        KcwSharedMemory<HANDLE> m_contentCheck;

        InputReader m_input;
};

#endif /* clienthandler_h */
