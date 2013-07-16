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
        OutputWriter();

        virtual void init();

        void setProcess(KcwProcess* proc);
        virtual void quit();

    private:
        COORD bufferSize() const;

    protected:
        KcwSharedMemory<CHAR_INFO> m_output;
        KcwSharedMemory<COORD> m_bufferSize;
        KcwNotifier m_bufferChanged;
        KcwNotifier m_bufferSizeChanged;
        KcwNotifier m_exitEventOutput;

        KcwProcess* m_process;
        friend class KcwSH::Terminal;
};
};
#endif /* outputwriter */