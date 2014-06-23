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
#ifndef kcwsh_terminal
#define kcwsh_terminal

#include <iostream>
#include <string>

#include <kcwthread.h>
#include <kcwprocess.h>
#include <kcwnotifier.h>
#include <windows.h>

#include "kcwsh_export.h"

namespace KcwSH {

class InputReader;
class OutputWriter;

class KCWSH_EXPORT Terminal : public KcwThread {
    public:
        Terminal();
        ~Terminal();

        void setCmd(const std::wstring& _cmd);
        std::wstring cmd() const;

        void setInputReader(InputReader* reader);
        void setOutputWriter(OutputWriter* writer);
        InputReader* inputReader() const;
        OutputWriter* outputWriter() const;

        bool isSetup() const;

        /**
         * this function closes down the complete terminal and shuts down the terminal thread
         */
        virtual KCW_CALLBACK(Terminal, quit);

        virtual KCW_CALLBACK(Terminal, sizeChanged);
        virtual KCW_CALLBACK(Terminal, bufferChanged);
        virtual KCW_CALLBACK(Terminal, cursorPositionChanged);
        virtual KCW_CALLBACK(Terminal, hasScrolled);

        /**
         * this callback will be called as soon as the terminal process has finished;
         * a finished terminal thread will automatically end this Terminal's thread
         * it won't be called in case you call quit directly
         */
        virtual KCW_CALLBACK(Terminal, hasQuit);

        virtual KCW_CALLBACK(Terminal, aboutToQuit);

        virtual KCW_CALLBACK(Terminal, activate);
        virtual KCW_CALLBACK(Terminal, deactivate);

        virtual KCW_CALLBACK(Terminal, titleChanged);

        void setActive(bool t);
        bool active() const;

        void setTerminalSize(COORD c);
        COORD terminalSize() const;

        COORD scrolledDistance(bool reset = true) const;

        DWORD run();

        void sendText(const std::wstring& t);
        void sendCommand(const std::wstring& c);

        void setTitle(const std::wstring& t);
        std::wstring title() const;

        int pid() const;
        int foregroundPid() const;

        void setInitialWorkingDirectory(const std::wstring& iwd);

        void setEnvironment(KcwProcess::KcwProcessEnvironment env);

    protected:
        Terminal(InputReader* ir, OutputWriter* ow);

    private:
        std::wstring getModulePath(HMODULE hModule);
        KCW_CALLBACK(Terminal, inputThreadDetached);
        KCW_CALLBACK(Terminal, outputThreadDetached);

        bool m_setup;
        bool m_active;

        HANDLE m_inputWriter;
        HANDLE m_outputReader;
        COORD m_termSize;

        InputReader *m_inputReader;
        OutputWriter *m_outputWriter;
        KcwProcess m_process;
        KcwNotifier m_exitEvent;
        KcwNotifier m_setupEvent;
        KcwNotifier m_titleChanged;
};
};
#endif /* kcwsh_terminal */
