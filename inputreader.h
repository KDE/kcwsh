#ifndef inputreader
#define inputreader

#include <windows.h>
#include <string>

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

        void sendText(const std::wstring& t);
        void sendCommand(const std::wstring& c);
        bool sendKeyboardEvents(INPUT_RECORD* ir, int len);
    protected:
        KcwNotifier m_exitEventInput;
        int m_cacheSize;
    private:
        KcwProcess* m_process;
        KcwNotifier m_readyRead;
        KcwNotifier m_bytesWritten;
        KcwSharedMemory<INPUT_RECORD> m_input;
        KcwSharedMemory<int> m_inputSize;
};
};
#endif /* inputreader */
