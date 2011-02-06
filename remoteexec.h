#ifndef remoteexec_h
#define remoteexec_h

#include "sharedmemory.h"

class RemoteExec {
    public:
        RemoteExec();
        // open the connections
        static void openConnections();
        
        // all the static callback functions
        static void bufferSizeCallback();
        
        // getters for the notification events
        static HANDLE bufferSizeNotification();
    private:
        static SharedMemory<int> m_bufferSize;
};

#endif /* remoteexec_h */