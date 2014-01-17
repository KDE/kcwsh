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
#ifndef outputwriter
#define outputwriter

#include <kcwthread.h>
#include <kcwnotifier.h>
#include <kcwsharedmemory.h>
#include <windows.h>

#include "kcwsh_export.h"

class KcwProcess;

namespace KcwSH {
class Terminal;

class KCWSH_EXPORT OutputWriter : public KcwThread {
    public:
        OutputWriter(Terminal* t);

        virtual void init();

        void setProcess(KcwProcess* proc);
        virtual void quit();
        virtual KCW_CALLBACK(OutputWriter, sizeChanged);
        virtual KCW_CALLBACK(OutputWriter, bufferChanged);
        virtual KCW_CALLBACK(OutputWriter, cursorPositionChanged);
        virtual KCW_CALLBACK(OutputWriter, hasScrolled);

        COORD scrolledDistance(bool reset = true) const;

        COORD bufferSize() const;
        void setBufferSize(COORD c);

        COORD cursorPosition() const;
        void setCursorPosition(COORD c);

        void setTitle(const std::wstring& t);
        std::wstring title() const;

        WCHAR at(COORD c) const;
        WORD attributesAt(COORD c) const;

        int foregroundPid() const;
    protected:
        KcwSharedMemory<CHAR_INFO> m_output;
        KcwSharedMemory<COORD> m_bufferSize;
        KcwSharedMemory<COORD> m_cursorPosition;
        KcwSharedMemory<COORD> m_scrolledDistance;
        KcwSharedMemory<WCHAR> m_title;
        KcwSharedMemory<DWORD> m_foregroundPid;

        KcwNotifier m_bufferChanged;
        KcwNotifier m_bufferSizeChanged;
        KcwNotifier m_cursorPositionChanged;
        KcwNotifier m_exitEventOutput;
        KcwNotifier m_shutdownEvent;
        KcwNotifier m_setupEvent;
        KcwNotifier m_scrollEvent;
        KcwNotifier m_titleChangeRequested;
        KcwNotifier m_titleChanged;

        HANDLE m_mutex;

        KcwProcess* m_process;
        Terminal* m_term;
};
};
#endif /* outputwriter */