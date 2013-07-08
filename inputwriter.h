#ifndef inputwriter
#define inputwriter

#include <kcweventloop.h>
#include <kcwnotifier.h>
#include <kcwsharedmemory.h>

#include <windows.h>

class InputWriter : public KcwEventLoop {
    public:
        InputWriter();

        void init();
        KCW_CALLBACK(InputWriter, writeData)
        void start();
    private:
        KcwNotifier m_bytesWritten;
        KcwNotifier m_readyRead;
        KcwSharedMemory<INPUT_RECORD> m_input;
        KcwSharedMemory<int> m_inputSize;
};
#endif /* inputwriter */