#ifndef pipehandler_h
#define pipehandler_h

#include <windows.h>

class PipeHandler {
    public:
        typedef enum {  STDIN_PIPE,
                        STDOUT_PIPE,
                        STDERR_PIPE
                     } STREAM_TYPE;
        PipeHandler( STREAM_TYPE stt );
        void read();
        void write();
        HANDLE readHandle();
        HANDLE writeHandle();
    private:
        static SECURITY_ATTRIBUTES s_saAttr;
        STREAM_TYPE m_streamType;
        HANDLE m_read;
        HANDLE m_write;
};
#endif /* pipehandler_h */
