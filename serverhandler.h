#ifndef serverhandler_h
#define serverhandler_h

#include "sharedmemory.h"

class ServerHandler {
    public:
        ServerHandler();
        void connect();
        void disconnect();
        
    private:
        SharedMemory<int> m_test;
};

#endif /* serverhandler_h */
