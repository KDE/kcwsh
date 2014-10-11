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

void OutputWriter::hasScrolled() {
//     KcwDebug() << __FUNCTION__;
    m_term->hasScrolled();
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
    m_sizeChangeRequested.notify();
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

WCHAR OutputWriter::historyAt(COORD c) const {
    CHAR_INFO ci;
    COORD bufSize;

    KcwAutoMutex a(m_mutex);
    a.lock();

    memcpy(&bufSize, m_bufferSize.data(), sizeof(COORD));
    memcpy(&ci, m_outputHistory.data() + (bufSize.X * c.X + c.Y), sizeof(CHAR_INFO));
    return ci.Char.UnicodeChar;
}

WORD OutputWriter::historyAttributesAt(COORD c) const {
    CHAR_INFO ci;
    COORD bufSize;

    KcwAutoMutex a(m_mutex);
    a.lock();

    memcpy(&bufSize, m_bufferSize.data(), sizeof(COORD));
    memcpy(&ci, m_outputHistory.data() + (bufSize.X * c.X + c.Y), sizeof(CHAR_INFO));
    return ci.Attributes;
}

COORD OutputWriter::scrolledDistance(bool reset) const {
    COORD c;
    ZeroMemory(&c, sizeof(COORD));
    KcwAutoMutex a(m_mutex);
    a.lock(__FUNCTION__);
    c = *m_scrolledDistance;
    if(reset) {
        COORD n;
        ZeroMemory(&n, sizeof(COORD));
        *m_scrolledDistance = n;
    }
    return c;
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
    wss << L"kcwsh-sizeChanged-" << m_process->pid();
    if(m_sizeChanged.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to open sizeChanged notifier:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-sizeChangeRequested-" << m_process->pid();
    if(m_sizeChangeRequested.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to open sizeChangeRequested notifier:" << wss.str();
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
    wss << L"kcwsh-scrollEvent-" << m_process->pid();
    if(m_scrollEvent.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to open scrollEvent notifier:" << wss.str();
        return;
    }

/*    wss.str(L"");
    wss << L"kcwsh-titleChanged-" << m_process->pid();
    if(m_titleChanged.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to open titleChanged notifier:" << wss.str();
        return;
    }
*/
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
    wss << L"kcwsh-outputHistory-" << m_process->pid();
    if(m_outputHistory.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to open outputHistory shared memory:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-title-" << m_process->pid();
    if(m_title.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to open title shared memory:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-scrolledDistance-" << m_process->pid();
    if(m_scrolledDistance.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to open scrolledDistance shared memory:" << wss.str();
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

    addCallback(m_sizeChanged, CB(sizeChanged));
    addCallback(m_bufferChanged, CB(bufferChanged));
    addCallback(m_cursorPositionChanged, CB(cursorPositionChanged));
    addCallback(m_scrollEvent, CB(hasScrolled));
}
