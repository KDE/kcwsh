#ifndef outputwriter
#define outputwriter

#include <kcwthread.h>
#include <kcwnotifier.h>
#include <kcwsharedmemory.h>
#include <windows.h>

#include "kcwsh_export.h"

class KcwProcess;

namespace KcwSH {
class Terminal;

class KCWSH_EXPORT OutputWriter : public KcwThread {
    public:
        OutputWriter(Terminal* t);

        virtual void init();

        void setProcess(KcwProcess* proc);
        virtual void quit();
        virtual KCW_CALLBACK(OutputWriter, sizeChanged);
        virtual KCW_CALLBACK(OutputWriter, bufferChanged);
        virtual KCW_CALLBACK(OutputWriter, cursorPositionChanged);

        COORD bufferSize() const;
        void setBufferSize(COORD c);

        void setTitle(const std::wstring& t);
        std::wstring title() const;

        int foregroundPid() const;
    protected:
        KcwSharedMemory<CHAR_INFO> m_output;
        KcwSharedMemory<COORD> m_bufferSize;
        KcwSharedMemory<COORD> m_cursorPosition;
        KcwSharedMemory<WCHAR> m_title;
        KcwSharedMemory<DWORD> m_foregroundPid;
        KcwNotifier m_bufferChanged;
        KcwNotifier m_bufferSizeChanged;
        KcwNotifier m_cursorPositionChanged;
        KcwNotifier m_exitEventOutput;
        KcwNotifier m_shutdownEvent;
        KcwNotifier m_setupEvent;
        KcwNotifier m_titleChangeRequested;
        KcwNotifier m_titleChanged;

        HANDLE m_mutex;

        KcwProcess* m_process;
        Terminal* m_term;
};
};
#endif /* outputwriter */