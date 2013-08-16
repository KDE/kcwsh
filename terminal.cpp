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

Terminal::Terminal()
: m_process("cmd.exe")
, m_setup(false)
, KcwThread() {
}

Terminal::Terminal(InputReader* ir, OutputWriter* ow)
: m_process("cmd.exe")
, m_setup(false)
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

void Terminal::setCmd(const std::string& _cmd) {
    m_process.setCmd(_cmd);
}

std::string Terminal::cmd() const {
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


COORD Terminal::terminalSize() const {
    COORD ret = {};
    if(isSetup()) {
        KcwDebug() << "isSetup!";
        return m_outputWriter->bufferSize();
    } else {
        return ret;
    }
}

void Terminal::sizeChanged() {
    KcwDebug() << "bufferSize has changed";
}

void Terminal::bufferChanged() {
    KcwDebug() << "buffer has changed";
}

void Terminal::cursorPositionChanged() {
    KcwDebug() << __FUNCTION__;
}

void Terminal::hasQuit() {
    Terminal::quit();
}

void Terminal::aboutToQuit() {
    KcwDebug() << "aboutToQuit!";
}

void Terminal::quit() {
    KcwDebug() << "Terminal::quit";
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
    KcwDebug() << "inputThreadDetached!";
}

void Terminal::outputThreadDetached() {
    KcwDebug() << "outputThreadDetached!";
}

DWORD Terminal::run() {
    if(m_inputReader == NULL || m_outputWriter == NULL) {
        KcwDebug() << "no inputreader or outputwriter set!";
        return -1;
    }

    // 1) create a shell process in suspended mode (default)
    m_process.start();
    addCallback(m_process.process(), CB(hasQuit));

    m_inputReader->setProcess(&m_process);
    m_inputReader->init();
    m_inputReader->start();

    std::wstringstream wss;
    wss << "kcwsh-exitEvent-" << m_process.pid();
    if(m_exitEvent.open(wss.str().c_str()) != 0) {
        KcwDebug() << "could not create exitEvent!";
        return -1;
    };
    addCallback(m_exitEvent, CB(aboutToQuit), true);

    wss.str(L"");
    wss << "kcwsh-setup-" << m_process.pid();
    if(m_setupEvent.open(wss.str().c_str()) != 0) {
        KcwDebug() << "could not create setupEvent!";
        return -1;
    };

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
    HANDLE hRemoteInputThread = CreateRemoteThread(m_process.process(), NULL, 0, pfnThreadRoutine, remoteProcHandle, 0, NULL);
    addCallback(hRemoteInputThread, CB(inputThreadDetached), true);

    // 3) create a remote thread which does the handling of output
    pfnThreadRoutine = (LPTHREAD_START_ROUTINE)((char*)injector.baseAddress() + ((char*)kcwshOutputHook - (char*)s_module));
    HANDLE hRemoteOutputThread = CreateRemoteThread(m_process.process(), NULL, 0, pfnThreadRoutine, remoteProcHandle, 0, NULL);
    addCallback(hRemoteOutputThread, CB(outputThreadDetached), true);

    // wait for 10 seconds maximum for setup of the other side
    DWORD ret = WaitForSingleObject(m_setupEvent.handle(), 3000);
    if(ret != WAIT_OBJECT_0) {
        KcwDebug() << "the other side failed to signal that it loaded correctly!";
        return -3;
    }
    m_setup = true;

    m_outputWriter->setProcess(&m_process);
    m_outputWriter->init();
    m_outputWriter->start();

    m_process.resume();
    return KcwEventLoop::exec();
}
