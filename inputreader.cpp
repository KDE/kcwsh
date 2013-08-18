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
    KcwDebug() << "sending text:" << t;
    int len = t.length();

    // FIXME: handle pastes longer than 128 characters
    if(len > 64) len = 64;

    INPUT_RECORD ir[128];
    ZeroMemory(ir, sizeof(INPUT_RECORD) * 128);
    for(int i = 0; i < 128 && i < len * 2; i+=2) {
        KcwDebug() << "writing char:" << t[i / 2] << i;
        int scan = VkKeyScanW(t[i / 2]);
        unsigned vsc = MapVirtualKey(scan & 0xff, MAPVK_VK_TO_VSC);
        bool shift = scan & 0x100, ctrl = scan & 0x200, alt = scan & 0x400;

        ir[i].EventType = KEY_EVENT;
        ir[i].Event.KeyEvent.bKeyDown = TRUE;
        ir[i].Event.KeyEvent.wRepeatCount = 1;
        ir[i].Event.KeyEvent.wVirtualKeyCode = vsc;//event->nativeVirtualKey();
        ir[i].Event.KeyEvent.wVirtualScanCode = scan & 0xff;//event->nativeScanCode();
        ir[i].Event.KeyEvent.uChar.UnicodeChar = t[i / 2]; //static_cast<WCHAR>(event->text().utf16()[0]);
        ir[i].Event.KeyEvent.dwControlKeyState = shift * SHIFT_PRESSED |
                                                 ctrl  * LEFT_CTRL_PRESSED |
                                                 alt   * RIGHT_ALT_PRESSED; //event->nativeModifiers();
        memcpy(&ir[i+1], &ir[i], sizeof(INPUT_RECORD));
        ir[i+1].Event.KeyEvent.bKeyDown = FALSE;
    }
    memcpy(m_input.data(), ir, sizeof(INPUT_RECORD) * len * 2);
    *m_inputSize = len * 2;
    m_bytesWritten.notify();
}

void InputReader::sendCommand(const std::wstring& c) {
    // send the text and append an Enterkey
    sendText(c);
    // FIXME:send an enterkey here
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

