#ifndef serverhandler_h
#define serverhandler_h

#include "kcwsharedmemory.h"
#include "kcweventloop.h"
#include "kcwthread.h"

class ServerHandler : public KcwThread {
    public:
        ServerHandler();
        void connect();
        void disconnect();

    private:
        KcwSharedMemory<HANDLE> m_sharedExitEvent;
        HANDLE m_timer;
};

#endif /* serverhandler_h */
