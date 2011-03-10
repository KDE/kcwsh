#ifndef kcwthread_h
#define kcwthread_h

#include <windows.h>

#include "kcwsharedmemory.h"
#include "kcweventloop.h"

/**
* @author Patrick Spendrin
* @date 2011
*
* @brief This class is supposed to wrap thread handling.
*
* To use this class, inherit KcwThread and overload run() in your class.
* If you want to start your thread, just call start() (you don't need to
* overload that function). The default implementation just starts an eventloop.
*/

class KcwThread : public KcwEventLoop {
    public:
        /**
        * Constructor for a thread object. If you want to override the default
        * event used for signaling the end of the thread, add it as the parameter.
        * @param exitEventHandle    the event used for signaling the end of the
        *                           handle; if exitEventHandle is NULL, a new Event will
        *                           be created.
        * @warning signaling the exitEvent is currently not threadsave. You should
        * @warning not change the exitEvent status after you started the thread.
        */
        KcwThread(HANDLE exitEventHandle = NULL);

        /**
        * this function starts the thread. The function run() will be called in
        * the thread.
        */
        void start();

        /**
        * this function is called when the thread is running. There is an eventLoop
        * running per default. You can override this behaviour by overloading run().
        */
        virtual DWORD run();

        /**
        * this function returns the handle used for signaling the exitEvent. Since
        * currently there is no threadsave implementation available for signaling the
        * exitEvent, you should use this getter instead.
        */
        virtual HANDLE exitEvent();

        /**
        * this function sets the exitEvent handle
        */
        void setExitEvent(HANDLE exitEventHandle);
    private:
        static DWORD WINAPI monitorThreadStatic(LPVOID lpParameter);
        DWORD monitorThread();

        static int getUniqueCounter();
        static KcwSharedMemory<int> s_globalThreadCounter;

        HANDLE m_thread;
        HANDLE m_exitEvent;
};
#endif /* kcwthread_h */
