#include "pipehandler.h"

SECURITY_ATTRIBUTES PipeHandler::s_saAttr = {   sizeof(SECURITY_ATTRIBUTES),
                                                NULL,
                                                TRUE
                                            };

PipeHandler::PipeHandler( STREAM_TYPE stt ) {
    if ( ! CreatePipe(&m_read, &m_write, &s_saAttr, 0) ) {
        OutputDebugString("failed to create pipe!");
        return;
    }

    // Ensure the read handle to the pipe is not inherited.
    switch(stt) {
        case PipeHandler::STDIN_PIPE:
            if ( ! SetHandleInformation(m_write, HANDLE_FLAG_INHERIT, 0) )
                OutputDebugString("not inherited");
            break;
        case PipeHandler::STDOUT_PIPE:
        case PipeHandler::STDERR_PIPE:
            if ( ! SetHandleInformation(m_read, HANDLE_FLAG_INHERIT, 0) )
                OutputDebugString("not inherited");
            break;
    }
}

HANDLE PipeHandler::readHandle() {
    return m_read;
}

HANDLE PipeHandler::writeHandle() {
    return m_write;
}

void PipeHandler::read() {
}

void PipeHandler::write() {
}

