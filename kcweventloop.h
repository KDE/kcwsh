#ifndef kcweventloop_h
#define kcweventloop_h

#include <windows.h>
#include <vector>

typedef void (*eventCallback)(void);

class KcwEventLoop {
    /**
    * KcwEventLoop is the EventLoop base class which handles the Event distribution and the event loop.
    * If you want to override the default Application wide Event handle for this loop, give it as parameter
    */
    public:
        KcwEventLoop(HANDLE eventHandle = NULL);
        
        /**
        * This function can be used to add handles to wait for and a callback to be called 
        * when this specific handle got signaled. Since this is a primitive implementation,
        * calling addCallback after exec gives undefined behavior.
        */
        void addCallback(HANDLE hndl, eventCallback cllbck = NULL);
        
        /**
        * call this function to run the event loop and to get notified 
        */
        int exec();
        
        /**
        * quit the event loop
        */
        void quit();
    private:
        std::vector<eventCallback> m_callbacks;
        std::vector<HANDLE> m_handles;
        int m_refreshInterval;
        HANDLE m_eventHandle;
};

#endif /* kcweventloop_h */
