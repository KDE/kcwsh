#ifndef kcwapp_h
#define kcwapp_h

#include <windows.h>
#include <vector>

typedef void (*eventCallback)(void);

class KcwApp {
    /**
    * KcwApp is the Application class which handles the Event distribution and the event loop.
    */
    public:
        KcwApp();
        
        /**
        * This function can be used to add handles to wait for and a callback to be called 
        * when this specific handle got signaled. Since this is a primitive implementation,
        * calling addCallback after exec gives undefined behavior.
        */
        void addCallback(HANDLE hndl, eventCallback cllbck);
        
        /**
        * call this function to run the event loop and to get notified 
        */
        int exec();
        
        /**
        * quit the event loop
        */
        static void quit();
    private:
        std::vector<eventCallback> m_callbacks;
        std::vector<HANDLE> m_handles;
        int m_refreshInterval;
        static HANDLE s_appEvent;
};
#endif /* kcwapp_h */
