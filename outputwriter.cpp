#include "outputwriter.h"

#include <kcwdebug.h>
#include <kcwprocess.h>

#include "terminal.h"

using namespace KcwSH;

OutputWriter::OutputWriter(Terminal* term)
: m_term(term)
, KcwThread() {
}

void OutputWriter::setProcess(KcwProcess* proc) {
    m_process = proc;
}

void OutputWriter::quit() {
    m_exitEventOutput.notify();
    KcwEventLoop::quit();
}

void OutputWriter::sizeChanged() {
//     KcwDebug() << __FUNCTION__;
    m_term->sizeChanged();
}

void OutputWriter::bufferChanged() {
//     KcwDebug() << __FUNCTION__;
    m_term->bufferChanged();
}

void OutputWriter::cursorPositionChanged() {
    m_term->cursorPositionChanged();
}

COORD OutputWriter::bufferSize() const {
    COORD ret;
//     KcwDebug() << "before the crash:";
//     KcwDebug() << m_bufferSize.data()->X << "X" << m_bufferSize.data()->Y;
    memcpy(&ret, m_bufferSize.data(), sizeof(COORD));
    return ret;
}

void OutputWriter::setBufferSize(COORD c) {
    m_shutdownEvent.notify();
    if(WaitForSingleObject(m_setupEvent, 1000) != WAIT_OBJECT_0) {
        KcwDebug() << __FUNCTION__ << "failed to wait for setup event!";
        return;
    }

    if(WaitForSingleObject(m_mutex, 5000) != WAIT_OBJECT_0) {
        KcwDebug() << __FUNCTION__ << "error!";
        return;
    }
    KcwDebug() << __FUNCTION__ << c.X << "X" << c.Y;
    *m_bufferSize = c;
    KcwDebug() << "resizing output buffer:";
    KcwDebug() << m_output.resize(c.X * c.Y);
    ReleaseMutex(m_mutex);
    m_bufferSizeChanged.notify();
    KcwDebug() << __FUNCTION__ << "finished!";
}

void OutputWriter::init() {
//     KcwDebug() << __FUNCTION__;
    std::wstringstream wss;
    wss.str(L"");
    wss << L"kcwsh-bufferSize-" << m_process->pid();
    if(m_bufferSize.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to open bufferSize shared memory:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-cursorPosition-" << m_process->pid();
    if(m_cursorPosition.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to open cursorPosition shared memory:" << wss.str();
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
    wss << L"kcwsh-cursorPositionChanged-" << m_process->pid();
    if(m_cursorPositionChanged.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to open cursorPositionChanged notifier:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-shutdown-" << m_process->pid();
    if(m_shutdownEvent.open(wss.str().c_str())) {
        KcwDebug() << "failed to open shutdownEvent notifier:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-setup-" << m_process->pid();
    if(m_setupEvent.open(wss.str().c_str())) {
        KcwDebug() << "failed to open setupEvent notifier:" << wss.str();
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

    wss.str(L"");
    wss << L"kcwsh-bufferMutex-" << m_process->pid();
    if((m_mutex = OpenMutexW(MUTEX_ALL_ACCESS, FALSE, wss.str().c_str())) == NULL) {
        KcwDebug() << "could not open mutex:" << wss.str();
        return;
    }

//     addCallback(m_bufferSizeChanged, CB(sizeChanged));
    addCallback(m_bufferChanged, CB(bufferChanged));
    addCallback(m_cursorPositionChanged, CB(cursorPositionChanged));
}
