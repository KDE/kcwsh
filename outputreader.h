#ifndef outputreader
#define outputreader

#include <kcweventloop.h>
#include <kcwsharedmemory.h>
#include <kcwnotifier.h>

#include <windows.h>

class OutputReader : public KcwEventLoop {
    public:
        OutputReader();

        void init();

        COORD getConsoleSize() const;

        KCW_CALLBACK(OutputReader, readData)
    private:
        KcwSharedMemory<CHAR_INFO> m_output;
        KcwSharedMemory<COORD> m_bufferSize;

        KcwNotifier m_bufferChanged;
        KcwNotifier m_bufferSizeChanged;
        KcwNotifier m_setupEvent;
        KcwNotifier m_exitEventOutput;
        
        COORD m_bufferSizeCache;

        HANDLE m_timer;
        HANDLE m_consoleHdl;
};
#endif /* outputreader */