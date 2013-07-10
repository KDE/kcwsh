#include "outputreader.h"

#include <kcwdebug.h>
#include <windows.h>

OutputReader::OutputReader()
: m_consoleHdl(GetStdHandle(STD_OUTPUT_HANDLE))
, KcwEventLoop() {
}

COORD OutputReader::getConsoleSize() const {
    COORD ret;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(m_consoleHdl, &csbi);
    ret.X = csbi.srWindow.Right - csbi.srWindow.Left;
    ret.Y = csbi.srWindow.Bottom - csbi.srWindow.Top;
    return ret;
}

void OutputReader::init() {
    DWORD dwProcessId = ::GetCurrentProcessId();

    m_timer = CreateWaitableTimer(NULL, FALSE, NULL);
    LARGE_INTEGER li;
    long period = 10;
    li.QuadPart = period * -10000LL; // 10 milliseconds
    SetWaitableTimer(m_timer, &li, period, NULL, NULL, TRUE);

    std::wstringstream wss;
    wss.str(L"");
    wss << L"kcwsh-setup-" << dwProcessId;
    KcwDebug() << "opening setupEvent:" << wss.str();
    if(m_setupEvent.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to open setupEvent notifier:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-bufferSize-" << dwProcessId;
    if(m_bufferSize.create(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to create bufferSize shared memory:" << wss.str();
        return;
    }
    *m_bufferSize = getConsoleSize();
    wss.str(L"");
    wss << L"kcwsh-bufferChanged-" << dwProcessId;
    if(m_bufferChanged.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to open bufferChanged notifier:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-output-" << dwProcessId;
    if(m_output.create(wss.str().c_str(), m_bufferSize.data()->X * m_bufferSize.data()->Y) != 0) {
        KcwDebug() << "failed to create output shared memory:" << wss.str();
        return;
    }

    addCallback(m_timer, CB(OutputReader::readData));
    KcwDebug() << "notifying setupEvent:";
    m_setupEvent.notify();
}

void OutputReader::readData() {
    KcwDebug() << "reading new Data!";
}

