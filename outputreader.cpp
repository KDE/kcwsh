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
#include "outputreader.h"

#include <kcwdebug.h>
#include <kcwautomutex.h>
#include <windows.h>

static OutputReader* out_reader = NULL;
struct CONSOLE_FONT
{
    DWORD index;
    COORD dim;
};

BOOL (WINAPI *SetConsoleFont)(HANDLE, DWORD);
BOOL (WINAPI *GetConsoleFontInfo)(HANDLE, BOOL, DWORD, CONSOLE_FONT*);
DWORD (WINAPI *GetNumberOfConsoleFonts)();

template<typename pfn_t>
inline bool LoadFunc(HMODULE hmod, const char *name, pfn_t &fn)
{
    fn = (pfn_t)GetProcAddress(hmod, name);
    return fn != 0;
}


OutputReader::OutputReader()
: m_consoleHdl(GetStdHandle(STD_OUTPUT_HANDLE))
, KcwEventLoop() {
    // TODO: if this fails, say something
    minimizeConsoleFont();
    COORD c;
    c=GetConsoleFontSize(m_consoleHdl, 1);
}

OutputReader::~OutputReader() {
//     KcwDebug() << __FUNCTION__;
}

bool OutputReader::minimizeConsoleFont() {
    // Undocumented API's
    HMODULE hmod = ::GetModuleHandleA("KERNEL32.DLL");
    if (!hmod ||
        !LoadFunc(hmod, "SetConsoleFont", SetConsoleFont) ||
        !LoadFunc(hmod, "GetConsoleFontInfo", GetConsoleFontInfo) ||
        !LoadFunc(hmod, "GetNumberOfConsoleFonts", GetNumberOfConsoleFonts)) {
        KcwDebug() << "Failed to load API(s): " << ::GetLastError() << endl;
        return false;
    }

    // number of console fonts
    const DWORD MAX_FONTS = 40;
    DWORD num_fonts = GetNumberOfConsoleFonts();
    if (num_fonts > MAX_FONTS)
        num_fonts = MAX_FONTS;

    CONSOLE_FONT fonts[MAX_FONTS];
    ZeroMemory(fonts, sizeof(fonts));
    GetConsoleFontInfo(m_consoleHdl, 0, num_fonts, fonts);

    for (DWORD n = 0; n < num_fonts; ++n) {
        fonts[n].dim = GetConsoleFontSize(m_consoleHdl, fonts[n].index);

//         KcwDebug() << "(" << n << "):" << fonts[n].index << fonts[n].dim.X << fonts[n].dim.Y;
        if (fonts[n].dim.X == 4 && fonts[n].dim.Y == 6) {
            SetConsoleFont(m_consoleHdl, fonts[n].index);
            return true;
        }
    }

    KcwDebug() << "failed to find minizable console font!";
    return false;
}

/**
 * returns the size of the console view window
 */
COORD OutputReader::getConsoleSize() const {
    COORD ret;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    // the console size is only the window size, the rest is needed for
    // the generation of the history.
    ret.X = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    ret.Y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    return ret;
}

bool operator >= (COORD a, COORD b) {
    return a.X >= b.X || a.Y >= b.Y;
}

bool operator != (COORD a, COORD b) {
    return a.X != b.X || a.Y != b.Y;
}

/**
 * callback to cleanup this thread.
 */
void OutputReader::shutdown() {
    KcwAutoMutex a(m_mutex);
    a.lock(__FUNCTION__);
    m_output.close();
    a.unlock();
    m_setupEvent.notify();
}

/**
 * sets the size of view & buffer to the size currently contained in m_bufferSize
 */
void OutputReader::setConsoleSize() {
    // first, check if the buffer is still opened, as we can't resize an open buffer
    if(m_output.opened()) {
        KcwDebug() << __FUNCTION__ << "failed, buffer still opened!";
        return;
    }

    // the lock has to be this early, so that the resolutions etc. are still valid when we change them
    KcwAutoMutex a(m_mutex);
    a.lock(__FUNCTION__);

    // each console has a maximum size in lines/columns, which is calculated using
    // the current screen size and the size of the of the fonts so that the
    // console cannot be bigger than the current screen - that's why we have OutputReader::minimizeConsoleFont
    COORD maxSize = GetLargestConsoleWindowSize(GetStdHandle(STD_OUTPUT_HANDLE));
//     KcwDebug() << "maximumSize:" << maxSize.X << "X" << maxSize.Y;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

    // sr contains the four coordinates of the console view window (in lines & columns),
    // bufferSize contains the overall size of the buffer
    SMALL_RECT sr = csbi.srWindow;
    COORD bufferSize = csbi.dwSize;
    COORD oldSize = bufferSize;

    // check if newly requested size is totally contained in maxSize
    // if not restrict it to the maximum size
    if(*m_bufferSize >= maxSize) {
        if(m_bufferSize->X > maxSize.X) m_bufferSize->X = maxSize.X;
        if(m_bufferSize->Y > maxSize.Y) m_bufferSize->Y = maxSize.Y;
    }

    // Resizing a console window actually consists of two steps: adjusting the size of the buffer,
    // and adjusting the size of the window. Of course, the window (actually a view on the buffer)
    // can't be bigger than the buffer itself, so in case we want to increase the view size, we need to
    // increase the size of the buffer first. If we want to decrease the size of the view, we need to
    // first decrease the size of the view, and then decrease the size of the buffer too.


    // increasing the view requires a buffer increase first
    // X is the width (number of columns)
    if(oldSize.X < m_bufferSize->X) {
        bufferSize.X = m_bufferSize->X;
        if(!SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), bufferSize)) {
            DWORD dw = GetLastError();
            KcwDebug() << "failed to increase screen buffer width to" << bufferSize.X << "Error:" << dw;
        }
    }
    // Y is the height (number of lines)
    if(oldSize.Y < m_bufferSize->Y) {
        bufferSize.Y = m_bufferSize->Y;
        if(!SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), bufferSize)) {
            DWORD dw = GetLastError();
            KcwDebug() << "failed to increase screen buffer height to" << bufferSize.Y << "Error:" << dw;
        }
    }

    // now change the size of the view
    sr.Bottom = sr.Top + m_bufferSize->Y - 1;
    if(!SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &sr)) {
        DWORD dw = GetLastError();
        KcwDebug() << "failed to set console height!" << dw;
    }
    sr.Right = sr.Left + m_bufferSize->X - 1;
    if(!SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &sr)) {
        DWORD dw = GetLastError();
        KcwDebug() << "failed to set console width!" << dw;
    }

    // in the end, change the buffer size, if it gets decreased
    if(oldSize.X > m_bufferSize->X) {
        bufferSize.X = m_bufferSize->X;
        if(!SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), bufferSize)) {
            DWORD dw = GetLastError();
            KcwDebug() << "failed to decrease screen buffer width to" << bufferSize.X << "Error:" << dw;
        }
    }
    if(oldSize.Y > m_bufferSize->Y) {
        bufferSize.Y = m_bufferSize->Y;
        if(!SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), bufferSize)) {
            DWORD dw = GetLastError();
            KcwDebug() << "failed to decrease screen buffer height to" << bufferSize.Y << "Error:" << dw;
        }
    }

    // finally reopen the output
    m_output.open(m_output.name());
}

/**
 * returns the current position of the cursor in the view window.
 */
COORD OutputReader::getCursorPosition() const {
    COORD ret;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    ret.Y = csbi.dwCursorPosition.Y - csbi.srWindow.Top;
    ret.X = csbi.dwCursorPosition.X;
    return ret;
}

/**
 * function which is supposed to handle scrollevents. This function needs to be really fast
 * TODO
 */
void OutputReader::scrollHappened(int horizontal, int vertical) {
    KcwAutoMutex a(m_mutex);
    a.lock(__FUNCTION__);

    COORD c = *m_scrolledDistance;
    c.X += horizontal;
    c.Y += vertical;
    *m_scrolledDistance = c;
//     KcwDebug() << "scrolled horizontally:" << c.X << "and vertically:" << c.Y;
    a.unlock();
    m_scrollEvent.notify();
}

/**
 * static Windows event callback for console accessibility events
 * This currently handles scroll and cursor position changes.
 */
void OutputReader::HandleConsoleEvent(HWINEVENTHOOK hook, DWORD event, HWND hwnd,
                                        LONG idObject, LONG idChild, DWORD dwEventThread,
                                        DWORD dwmsEventTime) {
    switch(event) {
        case EVENT_CONSOLE_UPDATE_SCROLL: {
            out_reader->scrollHappened(idObject, idChild);
            break;
        }
        case EVENT_CONSOLE_CARET: {
            if(idObject == CONSOLE_CARET_VISIBLE) {
                COORD pos;
                memcpy(&pos, &idChild, sizeof(COORD));
//                 KcwDebug() << "cursor moved to " << pos.X << ", " << pos.Y;
                KcwAutoMutex a(out_reader->m_mutex);
                a.lock(__FUNCTION__);
                *out_reader->m_cursorPosition = pos;
                a.unlock();
                out_reader->m_cursorPositionChanged.notify();
            } else {
                KcwDebug() << "selections are currently not understood!";
            }
            break;
        }
        case EVENT_CONSOLE_LAYOUT: {
            COORD windowSize = out_reader->getConsoleSize();

            static COORD oldSize = { 1, 1 };
            if(oldSize != windowSize) {
                out_reader->m_sizeChanged.notify();
                oldSize = windowSize;
            }
        }
        default: {
//             KcwDebug()
            break;
        }
    };
}

/**
 * contains the handle for the windows event callback
 * TODO: this is probably not needed at all
 */
HANDLE OutputReader::g_hook = NULL;

/**
 * sets up shared memory and notifiers, registers a recurring timer for calling OutputReader::readData
 */
void OutputReader::init() {
    DWORD dwProcessId = ::GetCurrentProcessId();
    g_hook = SetWinEventHook(
    EVENT_CONSOLE_CARET, EVENT_CONSOLE_END_APPLICATION + 1,  // Range of events (4 to 5).
    NULL,                                                    // Handle to DLL.
    OutputReader::HandleConsoleEvent,                        // The callback.
    0, 0,                                                    // Process and thread IDs of interest (0 = all)
    WINEVENT_OUTOFCONTEXT);                                  // Flags.

    // we only ever allow one instance of the OutputReader to make it easier to access it from the windows handler
    // in case we would allow several instances, we probably need to cleanup wenn the reader is dismantled (TODO)
    out_reader = this;

    // create the recurring timer which is started later in this function
    m_timer = CreateWaitableTimer(NULL, FALSE, NULL);
    LARGE_INTEGER li;
    long period = 100;
    li.QuadPart = period * -10000LL; // 10 milliseconds

    // register notifiers and shared memory
    std::wstringstream wss;
    wss.str(L"");
    wss << L"kcwsh-setup-" << dwProcessId;
//     KcwDebug() << "opening setupEvent:" << wss.str();
    if(m_setupEvent.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to open setupEvent notifier:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-titleChanged-" << dwProcessId;
//     KcwDebug() << "opening titleChanged:" << wss.str();
    if(m_titleChanged.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to open titleChanged notifier:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-shutdown-" << dwProcessId;
//     KcwDebug() << "opening setupEvent:" << wss.str();
    if(m_shutdownEvent.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to open shutdownEvent notifier:" << wss.str();
        return;
    }
    addCallback(m_shutdownEvent, CB(shutdown));

    wss.str(L"");
    wss << L"kcwsh-bufferSize-" << dwProcessId;
    if(m_bufferSize.create(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to create bufferSize shared memory:" << wss.str();
        return;
    }
    *m_bufferSize = getConsoleSize();

    wss.str(L"");
    wss << L"kcwsh-cursorPosition-" << dwProcessId;
    if(m_cursorPosition.create(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to create cursorPosition shared memory:" << wss.str();
        return;
    }
    *m_cursorPosition = getCursorPosition();

    wss.str(L"");
    wss << L"kcwsh-scrolledDistance-" << dwProcessId;
    if(m_scrolledDistance.create(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to create scrolledDistance shared memory:" << wss.str();
        return;
    }
    COORD n;
    ZeroMemory(&n, sizeof(COORD));
    *m_scrolledDistance = n;

    wss.str(L"");
    wss << L"kcwsh-sizeChanged-" << dwProcessId;
    if(m_sizeChanged.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to open sizeChanged notifier:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-sizeChangeRequested-" << dwProcessId;
    if(m_sizeChangeRequested.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to open sizeChangeRequested notifier:" << wss.str();
        return;
    }
    addCallback(m_sizeChangeRequested, CB(setConsoleSize));

    wss.str(L"");
    wss << L"kcwsh-bufferChanged-" << dwProcessId;
    if(m_bufferChanged.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to open bufferChanged notifier:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-cursorPositionChanged-" << dwProcessId;
    if(m_cursorPositionChanged.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to open cursorPositionChanged notifier:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-scrollEvent-" << dwProcessId;
    if(m_scrollEvent.open(wss.str().c_str()) != 0) {
        KcwDebug() << "failed to open scrollEvent notifier:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-output-" << dwProcessId;
    if(m_output.create(wss.str().c_str(), m_bufferSize.data()->X * m_bufferSize.data()->Y) != 0) {
        KcwDebug() << "failed to create output shared memory:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-outputHistory-" << dwProcessId;
    if(m_outputHistory.create(wss.str().c_str(), m_bufferSize.data()->X * m_bufferSize.data()->Y) != 0) {
        KcwDebug() << "failed to create outputHistory shared memory:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-title-" << dwProcessId;
    // this is the maximum size (64KB)
    if(m_title.create(wss.str().c_str(), 4096) != 0) {
        KcwDebug() << "failed to create title shared memory:" << wss.str();
        return;
    }

    wss.str(L"");
    wss << L"kcwsh-foregroundPid-" << dwProcessId;
    if(m_foregroundPid.create(wss.str().c_str(), 1) != 0) {
        KcwDebug() << "failed to create foregroundPid shared memory:" << wss.str();
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

    // simply zero out the title
    ZeroMemory(m_title.data(), 4096);

    addCallback(m_exitEventOutput);
    ZeroMemory(m_output.data(), m_bufferSize.data()->X * m_bufferSize.data()->Y * sizeof(CHAR_INFO));

    // everything is set up, start the timer for readData
    SetWaitableTimer(m_timer, &li, period, NULL, NULL, TRUE);
    addCallback(m_timer, CB(readData));
//     KcwDebug() << "notifying setupEvent";
    m_setupEvent.notify();

}

void OutputReader::readData() {
//     KcwDebug() << "reading new Data!";
    if(!m_output.opened()) {
        KcwDebug() << __FUNCTION__ << "output is closed, leaving!";
        return;
    }

    // read the title here, it seems there is no notification when the title changes.
    static WCHAR title[4096];
    static int titleLength = 0;
    WCHAR t[4096];
    ZeroMemory(t, 4096);
    DWORD l = GetConsoleTitle(t, 4096);
    if(l != titleLength || memcmp(title, t, sizeof(WCHAR) * l) != 0) {
        KcwAutoMutex a(m_mutex);
        a.lock(__FUNCTION__);

        titleLength = l;
        memcpy(title, t, sizeof(WCHAR) * l);
        memcpy(m_title.data(), t, sizeof(WCHAR) * (l + 1));
        m_titleChanged.notify();
    }

    // get current console process list
    DWORD  *pids, plen = 10;
    pids = new DWORD[plen];
    DWORD pl = GetConsoleProcessList(pids, plen);
    if(pl >= plen) {
        delete[] pids;
        pids = new DWORD[pl];
        pl = GetConsoleProcessList(pids, pl);
    }
    if(pl > 0 && *m_foregroundPid != pids[pl - 1]) {
        KcwAutoMutex a(m_mutex);
        a.lock(__FUNCTION__);
        *m_foregroundPid = pids[pl - 1];
    }
    delete[] pids;

    COORD size = *m_bufferSize;
    COORD bufferOrigin; 
    bufferOrigin.X = 0; bufferOrigin.Y = 0;
    COORD windowSize = getConsoleSize();

    static COORD oldSize = windowSize;
    static CHAR_INFO *buffer = new CHAR_INFO[windowSize.X * windowSize.Y];
    if(windowSize != oldSize) {
        delete[] buffer;
        buffer = new CHAR_INFO[windowSize.X * windowSize.Y];
        oldSize = windowSize;
    }
    SMALL_RECT sr;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    std::wstringstream wss;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    sr = csbi.srWindow;

    ReadConsoleOutput(GetStdHandle(STD_OUTPUT_HANDLE), buffer, windowSize, bufferOrigin, &sr);
    if(memcmp(buffer, m_output.data(), sizeof(CHAR_INFO) * windowSize.X * windowSize.Y) != 0) {
        KcwAutoMutex a(m_mutex);
        a.lock(__FUNCTION__);
        memcpy(m_output.data(), buffer, sizeof(CHAR_INFO) * windowSize.X * windowSize.Y);
        m_bufferChanged.notify();
    };
}

