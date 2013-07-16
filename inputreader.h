#ifndef inputreader
#define inputreader

#include <windows.h>

#include <kcwthread.h>
#include <kcwnotifier.h>
#include <kcwsharedmemory.h>

#include "kcwsh_export.h"

class KcwProcess;

namespace KcwSH {

class KCWSH_EXPORT InputReader : public KcwThread {
    public:
        InputReader();
        void setProcess(KcwProcess *proc);
        virtual void quit();
        KcwProcess* process();
        virtual void init();
    protected:
        KcwProcess* m_process;
        KcwNotifier m_readyRead;
        KcwNotifier m_bytesWritten;
        KcwNotifier m_exitEventInput;
        KcwSharedMemory<INPUT_RECORD> m_input;
        KcwSharedMemory<int> m_inputSize;
        int m_cacheSize;
};
};
#endif /* inputreader */
