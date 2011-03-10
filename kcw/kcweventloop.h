#ifndef kcweventloop_h
#define kcweventloop_h

#include <windows.h>
#include <vector>

typedef void (*eventCallback)(void *obj);

template <typename T> class KcwSharedMemory;

class KcwEventLoop {
    /**
    * KcwEventLoop is the event loop base class which handles the event distribution.
    * If you want to override the default Application wide event handle for this loop, 
    * give it as parameter or set it before calling exec().
    */
    public:
        KcwEventLoop(HANDLE eventHandle = NULL);
        virtual ~KcwEventLoop();

        /**
        * This function can be used to add handles to wait for and a callback to be called 
        * when this specific handle got signaled. Since this is a primitive implementation,
        * calling addCallback after exec gives undefined behavior.
        */
        void addCallback(HANDLE hndl, eventCallback cllbck = NULL, void *callbackObject = NULL);

        /**
        * call this function to run the event loop and to get notified 
        */
        int exec();

        /**
        * quit the event loop
        */
        virtual void quit();

        /**
        * change the wait interval for each handle; the default value is 10 milliseconds.
        */
        void setRefreshInterval(int msecs);

        /**
        * return the currently set wait interval for each handle in milliseconds.
        */
        int refreshInterval() const;

        /**
        * sets the default event used to signal that the event loop should be quit.
        */
        virtual void setExitEvent(HANDLE event);

        /**
        * returns the default event that is used to signal that the event loop should be quit.
        */
        virtual HANDLE exitEvent();
    private:
        std::vector<eventCallback> m_callbacks;
        std::vector<void*> m_objects;
        std::vector<HANDLE> m_handles;
        int m_refreshInterval;
		const int m_eventLoopId;
        HANDLE m_eventHandle;
        CRITICAL_SECTION m_criticalSection;
        static int getUniqueCounter();
        static KcwSharedMemory<int> s_globalEventLoopCounter;
};

#define KCW_CALLBACK(class, name) \
static void name##static(void *obj) { reinterpret_cast<##class##*>(obj)->##name##(); }\
void  name##();
#define CB(name) &name##static
#endif /* kcweventloop_h */
