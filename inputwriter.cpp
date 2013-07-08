#include "inputwriter.h"
#include <string>

#include <kcwdebug.h>

InputWriter::InputWriter() : KcwEventLoop() {
}

void InputWriter::writeData() {
//     const char* input = "dir\n";
    DWORD num = 0, len = 4;
/*    for(int i = 0; i < len; i++) {
        char vkc = toupper(input[i]);
        if(input[i] == '\n') vkc = VK_RETURN;
        PostMessage(hwnd, WM_KEYDOWN, vkc, 0x001C0001);
        PostMessage(hwnd, WM_KEYUP, vkc, 0xC01C0001);
    };*/
    KcwDebug() << "we wrote" << num << "events.";
    m_readyRead.notify();
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

    KcwDebug() << "adding new callback:" << m_bytesWritten.handle() << "from object:" << this;
     addCallback(m_bytesWritten.handle(), CB(InputWriter::writeData));
//    m_readyRead.notify();
}
