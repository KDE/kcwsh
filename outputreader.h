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
#ifndef outputreader
#define outputreader

#include <kcweventloop.h>
#include <kcwsharedmemory.h>
#include <kcwnotifier.h>

#include <windows.h>

class OutputReader : public KcwEventLoop {
    public:
        OutputReader();
        ~OutputReader();

        void init();

        COORD getConsoleSize() const;
        COORD getCursorPosition() const;

        KCW_CALLBACK(OutputReader, readData);
        KCW_CALLBACK(OutputReader, setConsoleSize);
        KCW_CALLBACK(OutputReader, adaptBufferSize);
        KCW_CALLBACK(OutputReader, shutdown);
        KCW_CALLBACK(OutputReader, setTitle);

        void scrollHappened(int horizontal, int vertical);
//         KCW_CALLBACK(OutputReader, scrollHappened);
    private:
        static void CALLBACK HandleConsoleEvent(HWINEVENTHOOK hook, DWORD event, HWND hwnd,
                                        LONG idObject, LONG idChild, DWORD dwEventThread,
                                        DWORD dwmsEventTime);
        bool minimizeConsoleFont();
        KcwSharedMemory<CHAR_INFO> m_output;
        KcwSharedMemory<COORD> m_bufferSize;
        KcwSharedMemory<COORD> m_cursorPosition;
        KcwSharedMemory<WCHAR> m_title;
        KcwSharedMemory<int> m_foregroundPid;
        KcwSharedMemory<COORD> m_scrolledDistance;

        KcwNotifier m_bufferChanged;
        KcwNotifier m_bufferSizeChanged;
        KcwNotifier m_cursorPositionChanged;
        KcwNotifier m_setupEvent;
        KcwNotifier m_shutdownEvent;
        KcwNotifier m_scrollEvent;
        KcwNotifier m_exitEventOutput;
        KcwNotifier m_titleChangeRequested;
        KcwNotifier m_titleChanged;

        COORD m_bufferSizeCache;

        HANDLE m_mutex;
        HANDLE m_timer;
        HANDLE m_consoleHdl;
        static HANDLE g_hook;
};
#endif /* outputreader */