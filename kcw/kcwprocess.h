#ifndef kcwprocess_h
#define kcwprocess_h

/**
* @author Patrick Spendrin
* @date 2011
*/

#include <windows.h>
#include <string>

class KcwProcess {
    /**
    * This class is used to abstract a process, either a running one or
    * to start a new one.
    */
    public:
        /**
        * start a new process using the executable @p execPath
        */
        KcwProcess(std::string execPath);

        /**
        * wrap an already running process with @p pid
        */
        KcwProcess(int pid);

        /**
        * the default constructor: you can set the executable path later
        */
        KcwProcess();

        typedef enum {  KCW_STDIN_HANDLE,
                        KCW_STDOUT_HANDLE,
                        KCW_STDERR_HANDLE
                     } KCW_STREAM_TYPE;

        /**
        * set a handle to one of input, output or error stream of the process.
        */
        void setStdHandle(HANDLE hdl, KCW_STREAM_TYPE type);
        
        /**
        * when starting a process, you can start it in a paused state and resume() it later
        */
        void setIsStartedAsPaused(bool isPaused);
        
        /**
        * when
        */
        void setExecutablePath(std::string execPath);
        void setStartupFlags(int stFlags);

        bool start();
        bool resume();


        int startupFlags() const;
        HANDLE process() const;
        HANDLE thread() const;
        bool isRunning() const;
        int pid() const;
        std::string executablePath() const;

    private:
        PROCESS_INFORMATION *m_procInfo;
        HANDLE              m_stdHandles[3];
        std::string         m_executablePath;
        int                 m_pid;
        int                 m_startupFlags;
        bool                m_isRunning;
        bool                m_isStartedAsPaused;
};


#endif /* kcwprocess_h */
