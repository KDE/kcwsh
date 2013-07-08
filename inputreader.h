#ifndef inputreader
#define inputreader

#include <windows.h>

#include <kcwthread.h>
#include <kcwnotifier.h>
#include <kcwsharedmemory.h>

class InputReader : public KcwThread {
    public:
        InputReader();
        void setProcess(int pid);
        void init(HANDLE _stdIn);

        KCW_CALLBACK(InputReader, transferData)
    private:
        int m_pid;
        KcwNotifier m_readyRead;
        KcwNotifier m_bytesWritten;
        KcwSharedMemory<INPUT_RECORD> m_input;
        KcwSharedMemory<int> m_inputSize;
        int m_bufferSize;

        HANDLE m_localConsole;
};
#endif /* inputreader */