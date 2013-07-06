#include "pipehandler.h"

#include <iostream>
#include <string>
#include <sstream>

#include <kcwdebug.h>

#define BUFSIZE 4096

static bool g_debug;

SECURITY_ATTRIBUTES PipeHandler::s_saAttr = {   sizeof(SECURITY_ATTRIBUTES),
                                                NULL,
                                                TRUE
                                            };

PipeHandler::PipeHandler( STREAM_TYPE stt ) {
    if ( ! CreatePipe(&m_read, &m_write, &s_saAttr, 0) ) {
        KcwDebug() << "failed to create pipe!";
        return;
    }

    // Ensure the read handle to the pipe is not inherited.
    switch(stt) {
        case PipeHandler::STDIN_PIPE:
            if ( ! SetHandleInformation(m_write, HANDLE_FLAG_INHERIT, 0) )
                KcwDebug() << "not inherited";
            break;
        case PipeHandler::STDOUT_PIPE:
        case PipeHandler::STDERR_PIPE:
            if ( ! SetHandleInformation(m_read, HANDLE_FLAG_INHERIT, 0) )
                KcwDebug() << "not inherited";
            break;
    }
}

HANDLE PipeHandler::readHandle() {
    return m_read;
}

HANDLE PipeHandler::writeHandle() {
    return m_write;
}

/************************************************************************************************
*
*************************************************************************************************/
InputPipe::InputPipe()
 : PipeHandler(PipeHandler::STDIN_PIPE) {
    addCallback(GetStdHandle(STD_INPUT_HANDLE), CB(transferStdIn), this);
}

void InputPipe::setTargetProcessId(int processId) {
    WCHAR tmp[1024];
    m_targetPid = processId;
    KcwDebug() << "trying to open";
    wsprintf(tmp, L"kcwsh-bufferSize-%x", m_targetPid);
    if(m_bufferSize.create(tmp) != 0) {
        m_bufferSize.errorExit();
    };
    KcwDebug() << tmp;
    KcwDebug() << "opened!";
}

void InputPipe::parseEscapeSequence(char *esc, int length) {
    char d1;
    if(esc[0] == '[') {
        KcwDebug() << "got escape sequence with suffix '" << esc[length - 1] << "' data: '" << &esc[1] << "'";
        switch(esc[length - 1]) {
            case 't':   {   // setting of the buffer size
                            std::istringstream i(&esc[1]);
                            if( i >> m_bufferSize->X >> d1 >> m_bufferSize->Y ){
//                                m_bufferSize.notify();
                            }
                            break;
                        }
            default:    {
                        };
        };
    }
}

void InputPipe::setContentCheckEvent(HANDLE evnt) {
    m_contentCheck = evnt;
}


// this needs to be moved into its own thread, otherwise it blocks to much
void InputPipe::transferStdIn() {
    DWORD dwRead = 0, dwWritten, dwResult, ret = 0;
    CHAR chBuf[BUFSIZE];
    BOOL bSuccess = FALSE;
    static char buffer[BUFSIZE];
    static int bufLength = 0;
    static int beginEsc = 0;
    static bool inEscapeSeq = false;

    KcwDebug() << "calling transferStdIn";

    dwResult = WaitForSingleObject(GetStdHandle(STD_INPUT_HANDLE), 0);
    
    if(dwResult == WAIT_OBJECT_0) {
        bSuccess = ReadFile(GetStdHandle(STD_INPUT_HANDLE), chBuf, BUFSIZE, &dwRead, NULL);
        KcwDebug() << "read string: ";
        for(unsigned i = 0; i < dwRead; i++) {
            buffer[bufLength + i] = chBuf[i];
//            wsprintf(tmp, L"%s %i", tmp, chBuf[i]);
        }
        bufLength += dwRead;
 //       OutputDebugString(tmp);
        if(!bSuccess) {
            KcwDebug() << "no success reading from stdin";
            return;
        }

        for(int j = 0; j < bufLength; j++) {
            // found the begin of an escape sequence
            if(!inEscapeSeq && (buffer[j] == 0x1b || 
               (g_debug && j > 1 && buffer[j - 2] == '\\' && buffer[j - 1] == '\\' && buffer[j] == 'e'))) {
                beginEsc = j + 1;
//                KcwDebug() << "inEscapeSequence";
                inEscapeSeq = true;
            }
            
            // found the end of an escape sequence
            if(inEscapeSeq && buffer[j] == 13) {
                buffer[j] = 0;
                parseEscapeSequence(&buffer[beginEsc], j - beginEsc);
//                KcwDebug() << "end inEscapeSequence";
                inEscapeSeq = false;
                if(buffer[beginEsc - 1] != 0x1b) {
                    bufLength = beginEsc - 3;
                } else {
                    bufLength = beginEsc - 1;
                }

                buffer[bufLength] = 0;
                return;
            }
        }
        
        if(!inEscapeSeq) {
            if(chBuf[0] == 13 && dwRead == 1) {
                chBuf[1] = 10;
                dwRead = 2;
            }
            SetEvent(m_contentCheck);
            bSuccess = WriteFile(writeHandle(), chBuf, dwRead, &dwWritten, NULL);
            if(!bSuccess) {
                KcwDebug() << "no success writing stdin to client";
                return;
            }
        }
    }
}

OutputPipe::OutputPipe() {
}

DWORD OutputPipe::run() {
    KcwDebug() << "OutputPipe::run";
    return exec();
}

void OutputPipe::bufferChanged() {
    KcwDebug() << "buffer changed!";
}

void OutputPipe::setTargetProcessId(int processId) {
    WCHAR tmp[1024];
    m_targetPid = processId;
    KcwDebug() << "trying to create buffer";
    wsprintf(tmp, L"kcwsh-buffer-%x", m_targetPid);
    if(m_buffer.create(tmp, 1000*1000) != 0) {
        m_buffer.errorExit();
    };
    
//    addCallback(m_buffer.notificationEvent(), CB(bufferChanged));
    KcwDebug() << tmp;
    KcwDebug() << "buffer opened!";
}

