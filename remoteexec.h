#ifndef remoteexec_h
#define remoteexec_h

#include <kcwsharedmemory.h>
#include <kcwnotifier.h>

class RemoteExec {
    public:
        RemoteExec();
        // open the connections
        static void openConnections();
        
        // all the static callback functions
//        static void bufferSizeCallback(void *obj);
        static void bufferContentCheck(void *obj);
        
        // getters for the notification events
//        static HANDLE bufferSizeNotification();
//        static HANDLE bufferNotification();
        static HANDLE contentNotification();
        static HANDLE exitEvent();
    private:
//        static KcwSharedMemory<COORD> s_bufferSize;
//        static KcwSharedMemory<CHAR_INFO> s_buffer;
        static KcwNotifier s_exitEvent;
        static KcwSharedMemory<HANDLE> s_contentCheck;
};

#endif /* remoteexec_h */
