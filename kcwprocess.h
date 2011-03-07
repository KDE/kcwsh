#ifndef kcwprocess_h
#define kcwprocess_h

/**
* @author Patrick Spendrin
* @date 2011
*/

#include <windows.h>
#include <string>

class KcwProcess {
    public:
        KcwProcess(std::string execPath);
        KcwProcess(int pid);
        KcwProcess();

        typedef enum {  KCW_STDIN_HANDLE,
                        KCW_STDOUT_HANDLE,
                        KCW_STDERR_HANDLE
                     } KCW_STREAM_TYPE;

        void setStdHandle(HANDLE hdl, KCW_STREAM_TYPE type);
        void setIsStartedAsPaused(bool isPaused);
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
