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

        COORD bufferSize() const;
    protected:
        KcwSharedMemory<CHAR_INFO> m_output;
        KcwSharedMemory<COORD> m_bufferSize;
        KcwNotifier m_bufferChanged;
        KcwNotifier m_bufferSizeChanged;
        KcwNotifier m_exitEventOutput;

        HANDLE m_mutex;

        KcwProcess* m_process;
        Terminal* m_term;
};
};
#endif /* outputwriter */