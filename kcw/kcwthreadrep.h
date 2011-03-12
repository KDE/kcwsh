#ifndef kcwthreadrep
#define kcwthreadrep

#include <windows.h>
#include <vector>

/**
* @author Patrick Spendrin
* @date 2011
* @brief a simple thread representation
* @details KcwThreadRep wraps a thread in any application and gives basic functionality.
*/
class KcwThreadRep {
    public:
        /**
        * create a default thread representation object.
        */
        KcwThreadRep();

        /**
        * create a thread representation for an application local thread with thread handle @p thread.
        * this is equivalent to calling KcwThreadRep ktr(GetCurrentProcess(void), thread); .
        */
        KcwThreadRep(HANDLE thread);

        /**
        * create a thread representation for a thread with thread handle @p thread in the application with
        * process handle @p app.
        */
        KcwThreadRep(HANDLE app, HANDLE thread);

        /**
        * convenience constructor; equivalent to @code KcwThreadRep(KcwThreadRep::getHandleForThreadId(threadid)) @endcode
        */
        KcwThreadRep(int threadId);

        /**
        * convenience constructor; equivalent to 
        * @code KcwThreadRep(KcwThreadRep::getHandleForThreadId(pid), 
        *                    KcwThreadRep::getHandleForThreadId(pid)) @endcode.
        */
        KcwThreadRep(int pid, int threadid);

        /**@{*/
        /**
        * attach to the thread with handle @p thread of the local application.
        */
        void attachThread(HANDLE thread);

        /**
        * attach to the thread with handle @p thread of the application with handle @p app.
        */
        void attachAppThread(HANDLE app, HANDLE thread);

        /**
        * convenience method for @see attachThread(HANDLE thread).
        */
        void attachThreadId(int threadId);
        /**@}*/

        /**
        * @return true in case the thread had been suspended and could be resumed, 
        * false otherwise.
        */
        bool resume();

        /**
        * @return true in case the thread had been running and could be suspended, 
        * false otherwise.
        */
        bool suspend();

        /**
        * @return the handle of the thread
        */
        HANDLE threadHandle() const;

        /**
        * @return the handle to the process this thread lives in
        */
        HANDLE processHandle() const;

        /**
        * @return representations of all threads in the process with the handle @p app
        */
        static std::vector<KcwThreadRep> threads(HANDLE app);

        /**
        * @return representations of all threads in the process with the process id @p pid
        */
        static std::vector<KcwThreadRep> threads(int pid);

        /**
        * @return the process handle for the application with the process id @p pid
        */
        static HANDLE getHandleForPid(int pid);

        /**
        * @return the handle for the thread with thread id @p threadid
        */
        static HANDLE getHandleForThreadId(int threadid);
    private:
        HANDLE m_process;
        HANDLE m_thread;
};

#endif /* kcwthreadrep */
