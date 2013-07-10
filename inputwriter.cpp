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

    WriteConsoleInput(m_consoleHdl, field, len, &num);
//     KcwDebug() << "we wrote" << num << "events.";
    delete[] field;
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
