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

COORD OutputWriter::cursorPosition() const {
    COORD ret;
    memcpy(&ret, m_cursorPosition.data(), sizeof(COORD));
    return ret;
}

void OutputWriter::setCursorPosition(COORD c) {
    if(WaitForSingleObject(m_mutex, 5000) != WAIT_OBJECT_0) {
        KcwDebug() << __FUNCTION__ << "error!";
        return;
    }
    *m_cursorPosition = c;
    ReleaseMutex(m_mutex);
    m_cursorPositionChanged.notify();
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

WCHAR OutputWriter::at(COORD c) const {
    CHAR_INFO ci;
    if(WaitForSingleObject(m_mutex, 1000) != WAIT_OBJECT_0) {
        KcwDebug() << __FUNCTION__ << "error!";
        return 0;
    }
    COORD bufSize;
    memcpy(&bufSize, m_bufferSize.data(), sizeof(COORD));
    memcpy(&ci, m_output.data() + (bufSize.X * c.X + c.Y), sizeof(CHAR_INFO));
    ReleaseMutex(m_mutex);
    return ci.Char.UnicodeChar;
}

WORD OutputWriter::attributesAt(COORD c) const {
    CHAR_INFO ci;
    if(WaitForSingleObject(m_mutex, 1000) != WAIT_OBJECT_0) {
        KcwDebug() << __FUNCTION__ << "error!";
        return 0;
    }
    COORD bufSize;
    memcpy(&bufSize, m_bufferSize.data(), sizeof(COORD));
    memcpy(&ci, m_output.data() + (bufSize.X * c.X + c.Y), sizeof(CHAR_INFO));
    ReleaseMutex(m_mutex);
    return ci.Attributes;
}



void OutputWriter::setTitle(const std::wstring& t) {
    if(WaitForSingleObject(m_mutex, 5000) != WAIT_OBJECT_0) {
        KcwDebug() << __FUNCTION__ << "error!";
        return;
    }
    memcpy(m_title.data(), t.data(), t.length());
    ReleaseMutex(m_mutex);
    m_titleChangeRequested.notify();
}

std::wstring OutputWriter::title() const {
    std::wstring ret;
    if(WaitForSingleObject(m_mutex, 5000) != WAIT_OBJECT_0) {
        KcwDebug() << __FUNCTION__ << "error!";
        return std::wstring();
    }
    ret = std::wstring(m_title.data());
    ReleaseMutex(m_mutex);
    return ret;
}

int OutputWriter::foregroundPid() const {
    int ret;
    if(WaitForSingleObject(m_mutex, 5000) != WAIT_OBJECT_0) {
        KcwDebug() << __FUNCTION__ << "error!";
        return 0;
    }
    ret = *m_foregroundPid.data();
    ReleaseMutex(m_mutex);
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
