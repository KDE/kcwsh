#include "outputwriter.h"

#include <kcwdebug.h>
#include <kcwprocess.h>

using namespace KcwSH;

OutputWriter::OutputWriter()
: KcwThread() {
}

void OutputWriter::setProcess(KcwProcess* proc) {
    m_process = proc;
}

void OutputWriter::quit() {
    m_exitEventOutput.notify();
    KcwDebug() << "notified outputreader";
    KcwDebug() << "quitting el:" << m_eventLoopId;
    KcwEventLoop::quit();
    KcwDebug() << "returned from own eventloop quit for OutputWriter";
}

COORD OutputWriter::bufferSize() const {
    COORD ret;
//     KcwDebug() << "before the crash:";
    KcwDebug() << m_bufferSize.data()->X << "X" << m_bufferSize.data()->Y;
    memcpy(&ret, m_bufferSize.data(), sizeof(COORD));
    return ret;
}

void OutputWriter::init() {
    KcwDebug() << "OutputWriter::init";
    std::wstringstream wss;
    wss.str(L"");
    wss << L"kcwsh-bufferSize-" << m_process->pid();
    if(m_bufferSize.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to open bufferSize shared memory:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-bufferSizeChanged-" << m_process->pid();
    if(m_bufferSizeChanged.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to open bufferSizeChanged notifier:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-bufferChanged-" << m_process->pid();
    if(m_bufferChanged.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to open bufferChanged notifier:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-exitEventOutput-" << m_process->pid();
    if(m_exitEventOutput.open(wss.str().c_str())) {
        KcwDebug() << "failed to open exitEventOutput notifier:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-output-" << m_process->pid();
    if(m_output.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to open output shared memory:" << wss.str();
        return;
    }
}
