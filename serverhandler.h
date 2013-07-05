#ifndef serverhandler_h
#define serverhandler_h

#include "kcweventloop.h"
#include "kcwthread.h"

class ServerHandler : public KcwThread {
    public:
        ServerHandler();
        void connect();
        void disconnect();

    private:
        HANDLE m_timer;
};

#endif /* serverhandler_h */
