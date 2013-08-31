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
    // cache size is 128 chars
    int remlen = t.length();

    INPUT_RECORD *ir = new INPUT_RECORD[t.length()];
    for(int i = 0; i < t.length(); i++) {
        ir[i].EventType = KEY_EVENT;
        ir[i].Event.KeyEvent.bKeyDown = TRUE;
        ir[i].Event.KeyEvent.wRepeatCount = 1;
        ir[i].Event.KeyEvent.uChar.UnicodeChar = t[i];
    }
    sendKeyboardEvents(ir, t.length());
    delete[] ir;
}

void InputReader::sendCommand(const std::wstring& c) {
    // send the text and append an Enterkey
    sendText(c);
    // FIXME:send an enterkey here
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

