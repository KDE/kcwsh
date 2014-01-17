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
#include "inputreader.h"

#include <kcwdebug.h>
#include <kcwprocess.h>

using namespace KcwSH;

InputReader::InputReader()
: m_cacheSize(128)
, m_process(NULL)
, KcwThread() {
}

void InputReader::setProcess(KcwProcess* proc) {
    m_process = proc;
}

void InputReader::quit() {
    m_exitEventInput.notify();
    KcwEventLoop::quit();
}

void InputReader::sendText(const std::wstring& t) {
    int returnCnt = 0, pos = 0;
    while(pos < t.length() && pos != std::wstring::npos) {
        pos = t.find(L'\n', pos);
        if(pos < t.length() && pos != std::wstring::npos) returnCnt++;
        pos++;
    }
    const unsigned remlen = t.length() + returnCnt;

    INPUT_RECORD *ir = new INPUT_RECORD[remlen];
    for(unsigned i = 0, j = 0; i < t.length(); i++) {
        if(t[i] == L'\n') {
            ir[j].EventType = KEY_EVENT;
            ir[j].Event.KeyEvent.bKeyDown = TRUE;
            ir[j].Event.KeyEvent.wRepeatCount = 1;
            ir[j++].Event.KeyEvent.uChar.UnicodeChar = VK_RETURN;
        }
        ir[j].EventType = KEY_EVENT;
        ir[j].Event.KeyEvent.bKeyDown = TRUE;
        ir[j].Event.KeyEvent.wRepeatCount = 1;
        ir[j++].Event.KeyEvent.uChar.UnicodeChar = t[i];
    }

    sendKeyboardEvents(ir, remlen);
    delete[] ir;
}

void InputReader::sendCommand(const std::wstring& c) {
    // send the text and append an Enterkey
    std::wstring s = c;
    if(s[c.length() - 2] != L'\n') s.append(L"\n");
    sendText(s);
}

bool InputReader::sendKeyboardEvents(INPUT_RECORD* ir, int len) {
    int currentIndex = 0;
    while(currentIndex < len) {
        DWORD ret = WaitForSingleObject(m_readyRead, 500);
        if(ret != WAIT_OBJECT_0) {
            KcwDebug() << "other side is not in readyRead state!";
            return false;
        }

        int curlen = ((len - currentIndex > 128) ? 128 : (len - currentIndex));
        memcpy(m_input.data(), ir + currentIndex, sizeof(INPUT_RECORD) * curlen);
        currentIndex += curlen;
        *m_inputSize = curlen;
        m_bytesWritten.notify();
    }
    return true;
}

void InputReader::sendCtrlC() {
    m_ctrlC.notify();
}

void InputReader::init() {
    std::wstringstream wss;
    wss << L"kcwsh-readyRead-" << m_process->pid();
    if(m_readyRead.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to create readyRead notifier:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-bytesWritten-" << m_process->pid();
    if(m_bytesWritten.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to create bytesWritten notifier:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-ctrlC-" << m_process->pid();
    if(m_ctrlC.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to create ctrlC notifier:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-exitEventInput-" << m_process->pid();
    if(m_exitEventInput.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to create exitEventInput notifier:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-input-" << m_process->pid();
    if(m_input.create(wss.str().c_str(), m_cacheSize) != 0) {
        KcwDebug() << "failed to create input shared memory:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-inputSize-" << m_process->pid();
    if(m_inputSize.create(wss.str().c_str(), 1) != 0) {
        KcwDebug() << "failed to create inputSize shared memory:" << wss.str();
        return;
    }
}

