#include "kcwthread.h"

#include <cstdio>
#include <process.h>

KcwSharedMemory<int> KcwThread::s_globalThreadCounter;

// default value for exitEventHandle is NULL, in that case we need to set a system wide unique name for the event
KcwThread::KcwThread(HANDLE exitEventHandle) : KcwEventLoop() {
    if(exitEventHandle != NULL) {
        setExitEvent(exitEventHandle);
    } else {
        char tmp[1024];
        sprintf(tmp, "KcwThread-%i", getUniqueCounter());
        setExitEvent(::CreateEventA(NULL, FALSE, FALSE, tmp));
    }
#if 1
    m_thread = ::CreateThread(NULL, 0, monitorThreadStatic, reinterpret_cast<void*>(this), CREATE_SUSPENDED, NULL);
#else
	m_thread = (HANDLE)_beginthreadex(NULL, 0, monitorThreadStatic, reinterpret_cast<void*>(this), CREATE_SUSPENDED, NULL);
#endif
}

// start the thread by resuming it
void KcwThread::start() {
    ::ResumeThread(m_thread);
}

// a static helper function that is called by the operating system
DWORD WINAPI KcwThread::monitorThreadStatic(LPVOID lpParameter) {
    char tmp[1024];
    if(lpParameter == 0) {
        sprintf(tmp, "monitorThreadStatic called with argument: %x", lpParameter);
        OutputDebugStringA(tmp);
        return 0;
    }
    KcwThread* pKcwThread = reinterpret_cast<KcwThread*>(lpParameter);
    sprintf(tmp, "monitorThreadStatic called with argument: %x", pKcwThread);
    OutputDebugStringA(tmp);
    
    const unsigned ret = pKcwThread->monitorThread();
//	_endthreadex(0);
	return ret;
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
    return KcwEventLoop::exec();
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
    if(s_globalThreadCounter.open(L"KcwThreadGlobal") != 0) {
        if(s_globalThreadCounter.create(L"KcwThreadGlobal", 1) != 0) {
            // in case of failure, exit the complete application
            s_globalThreadCounter.errorExit();
        } else {
            *s_globalThreadCounter = 0;
        }
    }

    // increase the counter by one, currently this is still not thread save
    WCHAR tmp[1024];
    wsprintf(tmp, L"opening global thread number %i", *s_globalThreadCounter);
    OutputDebugString(tmp);
    return (*s_globalThreadCounter)++;
}
