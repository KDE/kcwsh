/* Copyright 2013-2014  Patrick Spendrin <ps_ml@gmx.de>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "inputwriter.h"
#include <string>

#include <kcwdebug.h>

using namespace KcwSH;

InputWriter::InputWriter()
: m_consoleHdl(GetStdHandle(STD_INPUT_HANDLE))
, KcwEventLoop() {
}

void InputWriter::writeData() {
    DWORD num = 0, len = *m_inputSize.data();
    INPUT_RECORD *field = new INPUT_RECORD[len];

    memcpy(field, m_input.data(), len * sizeof(INPUT_RECORD));
    for(int i = 0; i < len; i++) {
//         KcwDebug() << __FUNCTION__
//                    << field[i].Event.KeyEvent.bKeyDown
//                    << field[i].Event.KeyEvent.wRepeatCount
//                    << field[i].Event.KeyEvent.wVirtualKeyCode
//                    << field[i].Event.KeyEvent.wVirtualScanCode
//                    << field[i].Event.KeyEvent.uChar.UnicodeChar
//                    << field[i].Event.KeyEvent.dwControlKeyState;
    }


    WriteConsoleInput(m_consoleHdl, field, len, &num);
//     KcwDebug() << "we wrote" << num << "events.";
    delete[] field;
    m_readyRead.notify();
}

void InputWriter::writeCtrlC() {
//     KcwDebug() << "send ctrl+C";
    GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0);
}

void InputWriter::init() {
    DWORD dwProcessId = ::GetCurrentProcessId();

    std::wstringstream wss;
    wss << L"kcwsh-readyRead-" << dwProcessId;
    if(m_readyRead.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to open readyRead notifier:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-bytesWritten-" << dwProcessId;
    if(m_bytesWritten.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to open bytesWritten notifier:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-ctrlC-" << dwProcessId;
    if(m_ctrlC.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to open ctrlC notifier:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-exitEventInput-" << dwProcessId;
    if(m_exitEventInput.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to open exitEventInput notifier:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-input-" << dwProcessId;
    if(m_input.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to open input shared memory:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-inputSize-" << dwProcessId;
    if(m_inputSize.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to open inputSize shared memory:" << wss.str();
        return;
    }

//     KcwDebug() << "adding new callback:" << m_bytesWritten.handle() << "from object:" << this;
    addCallback(m_bytesWritten, CB(InputWriter::writeData));
    addCallback(m_exitEventInput);
    addCallback(m_ctrlC, CB(InputWriter::writeCtrlC));
    m_readyRead.notify();
}
