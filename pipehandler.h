#ifndef pipehandler_h
#define pipehandler_h

#include <windows.h>

#include "kcwthread.h"
#include "kcwsharedmemory.h"

class PipeHandler : public KcwThread {
    public:
        typedef enum {  STDIN_PIPE,
                        STDOUT_PIPE,
                        STDERR_PIPE
                     } STREAM_TYPE;
        PipeHandler( STREAM_TYPE stt );
        HANDLE readHandle();
        HANDLE writeHandle();
    private:
        static SECURITY_ATTRIBUTES s_saAttr;
        STREAM_TYPE m_streamType;
        HANDLE m_read;
        HANDLE m_write;
};

class InputPipe : public PipeHandler {
    public:
        InputPipe();
        void setTargetProcessId(int processId);
        void setContentCheckEvent(HANDLE evnt);
    private:
        void parseEscapeSequence(char *esc, int length);
        KCW_CALLBACK(InputPipe, transferStdIn)
        int m_targetPid;
        KcwSharedMemory<COORD> m_bufferSize;
        HANDLE m_contentCheck;
};
#endif /* pipehandler_h */
