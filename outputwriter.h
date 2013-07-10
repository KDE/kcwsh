#ifndef outputwriter
#define outputwriter

#include <kcwthread.h>
#include <kcwnotifier.h>
#include <kcwsharedmemory.h>
#include <windows.h>

#include "kcwsh_export.h"

class KcwProcess;

namespace KcwSH {
class KCWSH_EXPORT OutputWriter : public KcwThread {
    public:
        OutputWriter();

        virtual void init();

        void setProcess(KcwProcess* proc);
    protected:
        KcwSharedMemory<CHAR_INFO> m_output;
        KcwSharedMemory<COORD> m_bufferSize;
        KcwNotifier m_bufferChanged;

        KcwProcess* m_process;
};
};
#endif /* outputwriter */