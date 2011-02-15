#include "kcwthread.h"

#include <cstdio>

int KcwThread::threadCount = 0;

KcwThread::KcwThread(HANDLE exitEventHandle) {
    if(exitEventHandle != NULL) {
        m_exitEvent = exitEventHandle;
    } else {
        char tmp[1024];
        threadCount++;
        sprintf(tmp, "KcwThread-%i", threadCount);
        m_exitEvent = ::CreateEvent(NULL, FALSE, FALSE, tmp);
    }
    m_thread = ::CreateThread(NULL, 0, monitorThreadStatic, reinterpret_cast<void*>(this), CREATE_SUSPENDED, NULL);
}

// start the thread by resuming it
void KcwThread::start() {
    ResumeThread(m_thread);
}

// a static helper function that is called by the operating system
DWORD WINAPI KcwThread::monitorThreadStatic(LPVOID lpParameter) {
    KcwThread* pKcwThread = reinterpret_cast<KcwThread*>(lpParameter);
    return pKcwThread->monitorThread();
}

// an internal function used to access private data members (to signal the event)
DWORD KcwThread::monitorThread() {
    DWORD dwThreadResult = 0;
    dwThreadResult = run();
    SetEvent(m_exitEvent);
    return dwThreadResult;
}

// this is the default implementation
DWORD KcwThread::run() {
    return 0;
}

// return the exitEvent
HANDLE KcwThread::exitEvent() const {
    return m_exitEvent;
}
