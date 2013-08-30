#ifndef outputreader
#define outputreader

#include <kcweventloop.h>
#include <kcwsharedmemory.h>
#include <kcwnotifier.h>

#include <windows.h>

class OutputReader : public KcwEventLoop {
    public:
        OutputReader();
        ~OutputReader();

        void init();

        COORD getConsoleSize() const;
        COORD getCursorPosition() const;

        KCW_CALLBACK(OutputReader, readData);
        KCW_CALLBACK(OutputReader, setConsoleSize);
        KCW_CALLBACK(OutputReader, adaptBufferSize);
        KCW_CALLBACK(OutputReader, shutdown);
    private:
        KcwSharedMemory<CHAR_INFO> m_output;
        KcwSharedMemory<COORD> m_bufferSize;
        KcwSharedMemory<COORD> m_cursorPosition;

        KcwNotifier m_bufferChanged;
        KcwNotifier m_bufferSizeChanged;
        KcwNotifier m_cursorPositionChanged;
        KcwNotifier m_setupEvent;
        KcwNotifier m_shutdownEvent;
        KcwNotifier m_exitEventOutput;

        COORD m_bufferSizeCache;

        HANDLE m_mutex;
        HANDLE m_timer;
        HANDLE m_consoleHdl;
};
#endif /* outputreader */