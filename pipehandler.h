#ifndef pipehandler_h
#define pipehandler_h

#include <windows.h>

#include "kcwthread.h"

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
		void setContentCheckEvent(HANDLE evnt);
    private:
        KCW_CALLBACK(InputPipe, transferStdIn)
		HANDLE m_contentCheck;
};
#endif /* pipehandler_h */
