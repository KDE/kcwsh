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
#include "terminal.h"

#include <kcwinjector.h>
#include <kcwdebug.h>
#include <kcweventloop.h>

#include <windows.h>

#include "hooks.h"
#include "inputreader.h"
#include "outputwriter.h"

extern HINSTANCE s_module;

using namespace KcwSH;

COORD termDefaultSize = { 80, 25 };

Terminal::Terminal()
: m_process(L"cmd.exe")
, m_setup(false)
, m_active(false)
, m_termSize(termDefaultSize)
, m_inputReader(NULL)
, m_outputWriter(NULL)
, KcwThread() {
}

Terminal::Terminal(InputReader* ir, OutputWriter* ow)
: m_process(L"cmd.exe")
, m_setup(false)
, m_active(false)
, m_termSize(termDefaultSize)
, m_inputReader(ir)
, m_outputWriter(ow)
, KcwThread() {
}

Terminal::~Terminal() {
}

std::wstring Terminal::getModulePath(HMODULE hModule) {
    WCHAR szModulePath[MAX_PATH + 1];
    ::ZeroMemory(szModulePath, (MAX_PATH + 1)*sizeof(WCHAR));

    ::GetModuleFileName(hModule, szModulePath, MAX_PATH);

    std::wstring strPath(szModulePath);

    return strPath.substr(0, strPath.rfind('\\'));
}

void Terminal::setCmd(const std::wstring& _cmd) {
    m_process.setCmd(_cmd);
}

std::wstring Terminal::cmd() const {
    return m_process.cmd();
}

void Terminal::setInputReader(InputReader* reader) {
    m_inputReader = reader;
}

void Terminal::setOutputWriter(OutputWriter* writer) {
    m_outputWriter = writer;
}

InputReader* Terminal::inputReader() const {
    return m_inputReader;
}

OutputWriter* Terminal::outputWriter() const {
    return m_outputWriter;
}

void Terminal::sendText(const std::wstring& t) {
    m_inputReader->sendText(t);
}

void Terminal::sendCommand(const std::wstring& c) {
    m_inputReader->sendCommand(c);
}

void Terminal::setTitle(const std::wstring& t) {
    // FIXME: make sure that the title is less then 4096 chars long
    // this is the maximum title length
//     KcwDebug() << __FUNCTION__ << t;
    m_outputWriter->setTitle(t);
}

std::wstring Terminal::title() const {
    if(isSetup()) {
        return m_outputWriter->title();
    } else {
        return std::wstring();
    }
}

COORD Terminal::terminalSize() const {
    COORD ret = {};
    if(isSetup()) {
        return m_outputWriter->bufferSize();
    } else {
        return ret;
    }
}

void Terminal::sizeChanged() {
//     KcwDebug() << __FUNCTION__;
}

void Terminal::bufferChanged() {
//     KcwDebug() << __FUNCTION__;
}

void Terminal::cursorPositionChanged() {
//     KcwDebug() << __FUNCTION__;
}

void Terminal::titleChanged() {
//     KcwDebug() << __FUNCTION__;
}

void Terminal::hasScrolled() {
//     KcwDebug() << __FUNCTION__;
}

void Terminal::hasQuit() {
    Terminal::quit();
}

void Terminal::aboutToQuit() {
//     KcwDebug() << __FUNCTION__;
}

void Terminal::quit() {
//     KcwDebug() << __FUNCTION__;
    removeCallback(m_process.process(), CB(hasQuit));
    if(m_inputReader != NULL && m_outputWriter != NULL) {
        m_inputReader->quit();
        m_outputWriter->quit();
    }
    m_process.quit();
    KcwEventLoop::quit();
}

bool Terminal::isSetup() const {
    return m_setup;
}

void Terminal::inputThreadDetached() {
//     KcwDebug() << "inputThreadDetached!";
}

void Terminal::outputThreadDetached() {
//     KcwDebug() << "outputThreadDetached!";
}
void Terminal::activate() {
    setActive(true);
}

void Terminal::deactivate() {
    setActive(false);
}

void Terminal::setTerminalSize(COORD c) {
//     KcwDebug() << __FUNCTION__ << c.X << "X" << c.Y;
    if(isSetup()) {
        m_outputWriter->setBufferSize(c);
    } else
        m_termSize = c;
}

void Terminal::setActive(bool t) {
    if(t == m_active) return;

    m_active = t;
    if(t) {
        ResumeThread(m_inputWriter);
        ResumeThread(m_outputReader);
    } else {
        SuspendThread(m_inputWriter);
        SuspendThread(m_outputReader);
    }
}

bool Terminal::active() const {
    return m_active;
}

int Terminal::pid() const {
    return m_process.pid();
}

int Terminal::foregroundPid() const {
    return m_outputWriter->foregroundPid();
}

COORD Terminal::scrolledDistance(bool reset) const {
    return m_outputWriter->scrolledDistance(reset);
}

void Terminal::setInitialWorkingDirectory(const std::wstring& iwd) {
    m_process.setInitialWorkingDirectory(iwd);
}

void Terminal::setEnvironment(KcwProcess::KcwProcessEnvironment env) {
    m_process.setStartupEnvironment(env);
}

DWORD Terminal::run() {
    if(m_inputReader == NULL || m_outputWriter == NULL) {
        KcwDebug() << "no inputreader or outputwriter set!";
        m_inputReader = new InputReader();
        m_outputWriter = new OutputWriter(this);
    }

    if(m_process.getStartupEnvironment().count(L"KCW_DEBUG") == 0)
    // we might not want to show our process ;-)
        m_process.setStartupAsHidden(true);

    // 1) create a shell process in suspended mode (default)
    m_process.start();
    addCallback(m_process.process(), CB(hasQuit));

    m_active = true;

    m_inputReader->setProcess(&m_process);
    m_inputReader->init();
    m_inputReader->start();

    std::wstringstream wss;
    wss << "kcwsh-exitEvent-" << m_process.pid();
    if(m_exitEvent.open(wss.str().c_str()) != 0) {
        KcwDebug() << "could not create exitEvent!";
        return -1;
    };
    addCallback(m_exitEvent, CB(aboutToQuit), NULL, true);

    wss.str(L"");
    wss << "kcwsh-setup-" << m_process.pid();
    if(m_setupEvent.open(wss.str().c_str()) != 0) {
        KcwDebug() << "could not create setupEvent!";
        return -1;
    };

    wss.str(L"");
    wss << "kcwsh-titleChanged-" << m_process.pid();
    if(m_titleChanged.open(wss.str().c_str()) != 0) {
        KcwDebug() << "could not create titleChanged!";
        return -1;
    };
    addCallback(m_titleChanged, CB(titleChanged));

    // 2) inject this very dll into that process
    KcwInjector injector;
    injector.setInjectionDll(getModulePath(NULL) + L"\\kcwsh.dll");
    injector.setDestinationProcess(m_process.process(), m_process.thread());

    if (!injector.inject()) {
        KcwDebug() << "failed to inject dll!";
        return -2;
    }

    // duplicate the handle so that we can hand it over to the remote threads
    // that way they can be shut down when their applications wants to quit
    HANDLE remoteProcHandle;
    DuplicateHandle(GetCurrentProcess(), m_process.process(), m_process.process(), &remoteProcHandle, 0, FALSE, DUPLICATE_SAME_ACCESS);

    // 3) create a remote thread which does the handling of input
    LPTHREAD_START_ROUTINE pfnThreadRoutine = NULL;
    pfnThreadRoutine = (LPTHREAD_START_ROUTINE)((char*)injector.baseAddress() + ((char*)kcwshInputHook - (char*)s_module));
    m_inputWriter = CreateRemoteThread(m_process.process(), NULL, 0, pfnThreadRoutine, remoteProcHandle, 0, NULL);
    addCallback(m_inputWriter, CB(inputThreadDetached), NULL, true);

    // 3) create a remote thread which does the handling of output
    pfnThreadRoutine = (LPTHREAD_START_ROUTINE)((char*)injector.baseAddress() + ((char*)kcwshOutputHook - (char*)s_module));
    m_outputReader = CreateRemoteThread(m_process.process(), NULL, 0, pfnThreadRoutine, remoteProcHandle, 0, NULL);
    addCallback(m_outputReader, CB(outputThreadDetached), NULL, true);

    // wait for 10 seconds maximum for setup of the other side
    DWORD ret = WaitForSingleObject(m_setupEvent, 3000);
    if(ret != WAIT_OBJECT_0) {
        KcwDebug() << "the other side failed to signal that it loaded correctly!";
        return -3;
    }
    m_setup = true;

    m_outputWriter->setProcess(&m_process);
    m_outputWriter->init();
    m_outputWriter->start();

    m_outputWriter->setBufferSize(m_termSize);
    sizeChanged();

    m_process.resume();
    return KcwEventLoop::exec();
}
