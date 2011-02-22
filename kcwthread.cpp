#include "kcwthread.h"

#include <cstdio>

int KcwThread::threadCount = 0;
KcwSharedMemory<int> KcwThread::s_globalThreadCounter;

// default value for exitEventHandle is NULL, in that case we need to set a system wide unique name for the event
KcwThread::KcwThread(HANDLE exitEventHandle) {
    if(exitEventHandle != NULL) {
        setExitEvent(exitEventHandle);
    } else {
        // this is not system wide unique :-(
        char tmp[1024];
        threadCount++;
        sprintf(tmp, "KcwThread-%i", getUniqueCounter());
        setExitEvent(::CreateEvent(NULL, FALSE, FALSE, tmp));
    }
    m_thread = ::CreateThread(NULL, 0, monitorThreadStatic, reinterpret_cast<void*>(this), CREATE_SUSPENDED, NULL);
}

// start the thread by resuming it
void KcwThread::start() {
    ::ResumeThread(m_thread);
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
    return exec();
}

// return the exitEvent
HANDLE KcwThread::exitEvent() {
    return m_exitEvent;
}

// set the exitEvent so we can reuse an external event
void KcwThread::setExitEvent(HANDLE exitEventHandle) {
    KcwEventLoop::setExitEvent(exitEventHandle);
    m_exitEvent = exitEventHandle;
}

int KcwThread::getUniqueCounter() {
    // we need to check that our threadCounter has started
    // KcwSharedMemory<T>::open() and KcwSharedMemory<T>::create()
    // return 0 in case they already have been opened.
    if(s_globalThreadCounter.open("KcwThreadGlobal") != 0) {
        if(s_globalThreadCounter.create("KcwThreadGlobal", 1) != 0) {
            // in case of failure, exit the complete application
            s_globalThreadCounter.errorExit();
        } else {
            *s_globalThreadCounter = 0;
        }
    }

    // increase the counter by one, currently this is still not thread save
    char tmp[1024];
    sprintf(tmp, "opening global thread number %i", *s_globalThreadCounter + 1);
    OutputDebugString(tmp);
    return (*s_globalThreadCounter)++;
}
