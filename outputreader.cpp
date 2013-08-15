#include "outputreader.h"

#include <kcwdebug.h>
#include <windows.h>

OutputReader::OutputReader()
: m_consoleHdl(GetStdHandle(STD_OUTPUT_HANDLE))
, KcwEventLoop() {
    m_bufferSizeCache.X = 1; m_bufferSizeCache.Y = 1;
}

OutputReader::~OutputReader() {
//     KcwDebug() << __FUNCTION__;
}

COORD OutputReader::getConsoleSize() const {
    COORD ret;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(m_consoleHdl, &csbi);
    ret.X = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    ret.Y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    return ret;
}

void OutputReader::init() {
    DWORD dwProcessId = ::GetCurrentProcessId();

    m_timer = CreateWaitableTimer(NULL, FALSE, NULL);
    LARGE_INTEGER li;
    long period = 100;
    li.QuadPart = period * -10000LL; // 10 milliseconds
    SetWaitableTimer(m_timer, &li, period, NULL, NULL, TRUE);

    std::wstringstream wss;
    wss.str(L"");
    wss << L"kcwsh-setup-" << dwProcessId;
//     KcwDebug() << "opening setupEvent:" << wss.str();
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
    wss << L"kcwsh-bufferSizeChanged-" << dwProcessId;
    if(m_bufferSizeChanged.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to open bufferSizeChanged notifier:" << wss.str();
        return;
    }

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

    wss.str(L"");
    wss << L"kcwsh-exitEventOutput-" << dwProcessId;
    if(m_exitEventOutput.open(wss.str().c_str())) {
        KcwDebug() << "failed to open exitEventOutput notifier:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-bufferMutex-" << dwProcessId;
    if((m_mutex = CreateMutexW(NULL, FALSE, wss.str().c_str())) == NULL) {
        KcwDebug() << "failed to create bufferMutex:" << wss.str();
        return;
    }

    addCallback(m_exitEventOutput);
    ZeroMemory(m_output.data(), m_bufferSize.data()->X * m_bufferSize.data()->Y * sizeof(CHAR_INFO));

    addCallback(m_timer, CB(OutputReader::readData));
    if(memcmp(&m_bufferSizeCache, m_bufferSize.data(), sizeof(COORD)) != 0) {
        m_bufferSizeCache = *m_bufferSize;
        m_bufferSizeChanged.notify();
    }

//     KcwDebug() << "notifying setupEvent";
    m_setupEvent.notify();
}

void OutputReader::readData() {
//     KcwDebug() << "reading new Data!";
    COORD size = *m_bufferSize;
    COORD bufferOrigin; bufferOrigin.X = 0; bufferOrigin.Y = 0;
    COORD bufferSize = getConsoleSize();
    static CHAR_INFO *buffer = new CHAR_INFO[bufferSize.X * bufferSize.Y];
    SMALL_RECT sr;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    std::wstringstream wss;
    GetConsoleScreenBufferInfo(m_consoleHdl, &csbi);
    sr = csbi.srWindow;
    ReadConsoleOutput(m_consoleHdl, buffer, bufferSize, bufferOrigin, &sr);
    if(memcmp(&m_bufferSizeCache, m_bufferSize.data(), sizeof(COORD)) != 0) {
        wss.str(L"");
        wss << L"kcwsh-bufferMutex-" << ::GetCurrentProcessId();
/*        m_mutex = OpenMutexW(MUTEX_ALL_ACCESS, FALSE, wss.str().c_str()); */
        KcwDebug() << "waiting for mutex!";
        WaitForSingleObject(m_mutex, INFINITE);
        m_bufferSizeCache = *m_bufferSize;
        m_bufferSizeChanged.notify();
        KcwDebug() << "bufferSize changed!";
        ReleaseMutex(m_mutex);
        // unlock here
    }

    if(memcmp(buffer, m_output.data(), sizeof(CHAR_INFO) * bufferSize.X * bufferSize.Y) != 0) {
        // lock here
        memcpy(m_output.data(), buffer, sizeof(CHAR_INFO) * bufferSize.X * bufferSize.Y);
        m_bufferChanged.notify();
//         KcwDebug() << "output buffer changed!";
        // unlock here
    };
}

