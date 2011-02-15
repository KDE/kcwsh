#ifndef serverhandler_h
#define serverhandler_h

#include "sharedmemory.h"
#include "kcweventloop.h"

class ServerHandler : public KcwEventLoop {
    public:
        ServerHandler();
        void connect();
        void disconnect();
        
    private:
        static DWORD WINAPI monitorThreadStatic(LPVOID lpParameter);
        DWORD monitorThread();
        
        HANDLE m_thread;
        SharedMemory<HANDLE> m_sharedExitEvent;
};

#endif /* serverhandler_h */