#include "inputreader.h"

#include <kcwdebug.h>

InputReader::InputReader()
: m_bufferSize(128)
, m_pid(0)
, KcwThread() {
    KcwDebug() << this << "InputReader";
}

void InputReader::setProcess(int pid) {
    m_pid = pid;
}

void InputReader::init(HANDLE _stdIn) {
    std::wstringstream wss;
    m_localConsole = _stdIn;
    wss << L"kcwsh-readyRead-" << m_pid;
    if(m_readyRead.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to create readyRead notifier:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-bytesWritten-" << m_pid;
    if(m_bytesWritten.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to create bytesWritten notifier:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-input-" << m_pid;
    if(m_input.create(wss.str().c_str(), m_bufferSize) != 0) {
        KcwDebug() << "failed to create input shared memory:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-inputSize-" << m_pid;
    if(m_inputSize.create(wss.str().c_str(), 1) != 0) {
        KcwDebug() << "failed to create inputSize shared memory:" << wss.str();
        return;
    }

    KcwDebug() << "adding callback:" << m_localConsole << (void*)CB(InputReader::transferData);
    addCallback(m_localConsole, CB(InputReader::transferData));
//    addCallback(m_readyRead.handle(), CB(transferData));
}

void InputReader::transferData() {
    if(!this) {
        KcwDebug() << "no inputreader instance!";
        return;
    }

    INPUT_RECORD *buffer = new INPUT_RECORD[m_bufferSize];
    DWORD len = 0;
    if(!ReadConsoleInput(m_localConsole, buffer, m_bufferSize, &len)) {
        KcwDebug() << "failed to read console input!";
    } else {
        KcwDebug() << "read" << len << "records from stdin";
        memcpy(m_input.data(), buffer, len*sizeof(INPUT_RECORD));
        *m_inputSize = len;
        m_bytesWritten.notify();
    }
    delete[] buffer;
}