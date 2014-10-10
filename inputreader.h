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
#ifndef inputreader
#define inputreader

#include <windows.h>
#include <string>

#include <kcwthread.h>
#include <kcwnotifier.h>
#include <kcwsharedmemory.h>

#include "kcwsh_export.h"

class KcwProcess;

namespace KcwSH {

class KCWSH_EXPORT InputReader : public KcwThread {
    public:
        InputReader();
        void setProcess(KcwProcess *proc);
        virtual void quit();
        KcwProcess* process();
        virtual void init();

        void sendText(const std::wstring& t);
        void sendCommand(const std::wstring& c);
        bool sendKeyboardEvents(INPUT_RECORD* ir, int len);
        void sendCtrlC();
        void setTitle(const std::wstring& t);
    protected:
        KcwNotifier m_exitEventInput;
        int m_cacheSize;
    private:
        KcwProcess* m_process;
        KcwNotifier m_readyRead;
        KcwNotifier m_bytesWritten;
        KcwNotifier m_ctrlC;
        KcwNotifier m_titleChangeRequested;
        KcwSharedMemory<WCHAR> m_title;
        KcwSharedMemory<INPUT_RECORD> m_input;
        KcwSharedMemory<int> m_inputSize;
        HANDLE m_mutex;
};
};
#endif /* inputreader */
