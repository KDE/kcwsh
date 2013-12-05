#ifndef inputwriter
#define inputwriter

#include <kcweventloop.h>
#include <kcwnotifier.h>
#include <kcwsharedmemory.h>

#include <windows.h>

namespace KcwSH {

class InputWriter : public KcwEventLoop {
    public:
        InputWriter();

        void init();
        KCW_CALLBACK(InputWriter, writeData);
        KCW_CALLBACK(InputWriter, writeCtrlC);
    private:
        HANDLE m_consoleHdl;
        KcwNotifier m_bytesWritten;
        KcwNotifier m_readyRead;
        KcwNotifier m_exitEventInput;
        KcwNotifier m_ctrlC;
        KcwSharedMemory<INPUT_RECORD> m_input;
        KcwSharedMemory<int> m_inputSize;
};
};
#endif /* inputwriter */