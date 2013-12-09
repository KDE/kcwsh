#include "outputwriter.h"

#include <kcwdebug.h>
#include <kcwprocess.h>
#include <kcwautomutex.h>

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

COORD OutputWriter::cursorPosition() const {
    COORD ret;
    memcpy(&ret, m_cursorPosition.data(), sizeof(COORD));
    return ret;
}

void OutputWriter::setCursorPosition(COORD c) {
    KcwAutoMutex a(m_mutex);
    a.lock(__FUNCTION__);
    *m_cursorPosition = c;
    a.unlock();
    m_cursorPositionChanged.notify();
}

COORD OutputWriter::bufferSize() const {
    COORD ret;
//     KcwDebug() << "before the crash:";
//     KcwDebug() << m_bufferSize.data()->X << "X" << m_bufferSize.data()->Y;
    KcwAutoMutex a(m_mutex);
    a.lock(__FUNCTION__);
    memcpy(&ret, m_bufferSize.data(), sizeof(COORD));
    return ret;
}

void OutputWriter::setBufferSize(COORD c) {
    m_shutdownEvent.notify();
    if(WaitForSingleObject(m_setupEvent, 1000) != WAIT_OBJECT_0) {
        KcwDebug() << __FUNCTION__ << "failed to wait for setup event!";
        return;
    }

    KcwAutoMutex a(m_mutex);
    a.lock(__FUNCTION__);

    *m_bufferSize = c;
    m_output.resize(c.X * c.Y);
    a.unlock();
    m_bufferSizeChanged.notify();
}

WCHAR OutputWriter::at(COORD c) const {
    CHAR_INFO ci;
    COORD bufSize;

    KcwAutoMutex a(m_mutex);
    a.lock();

    memcpy(&bufSize, m_bufferSize.data(), sizeof(COORD));
    memcpy(&ci, m_output.data() + (bufSize.X * c.X + c.Y), sizeof(CHAR_INFO));
    return ci.Char.UnicodeChar;
}

WORD OutputWriter::attributesAt(COORD c) const {
    CHAR_INFO ci;
    COORD bufSize;

    KcwAutoMutex a(m_mutex);
    a.lock();

    memcpy(&bufSize, m_bufferSize.data(), sizeof(COORD));
    memcpy(&ci, m_output.data() + (bufSize.X * c.X + c.Y), sizeof(CHAR_INFO));
    return ci.Attributes;
}



void OutputWriter::setTitle(const std::wstring& t) {
    KcwAutoMutex a(m_mutex);
    a.lock(__FUNCTION__);
    memcpy(m_title.data(), t.data(), t.length());
    a.unlock();
    m_titleChangeRequested.notify();
}

std::wstring OutputWriter::title() const {
    std::wstring ret;
    KcwAutoMutex a(m_mutex);
    a.lock(__FUNCTION__);
    ret = std::wstring(m_title.data());
    return ret;
}

int OutputWriter::foregroundPid() const {
    int ret = 0;
    KcwAutoMutex a(m_mutex);
    a.lock(__FUNCTION__);
    if(m_foregroundPid.opened())
        ret = *m_foregroundPid.data();
    return ret;
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
    wss << L"kcwsh-titleChangeRequested-" << m_process->pid();
    if(m_titleChangeRequested.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to open titleChangeRequested notifier:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-titleChanged-" << m_process->pid();
    if(m_titleChanged.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to open titleChanged notifier:" << wss.str();
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
    wss << L"kcwsh-title-" << m_process->pid();
    if(m_title.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to open title shared memory:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-foregroundPid-" << m_process->pid();
    if(m_foregroundPid.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to open foregroundPid shared memory:" << wss.str();
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
