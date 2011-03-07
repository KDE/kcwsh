#include "kcweventloop.h"

KcwSharedMemory<int> KcwEventLoop::s_globalEventLoopCounter;

KcwEventLoop::KcwEventLoop(HANDLE eventHandle)
 :  m_refreshInterval(10),
	m_eventLoopId(getUniqueCounter()) {
    InitializeCriticalSection(&m_criticalSection);
    EnterCriticalSection(&m_criticalSection);
    if(eventHandle != NULL) {
		m_eventHandle = eventHandle;
	} else {
        char tmp[1024];
        sprintf(tmp, "KcwEventLoop-%i", m_eventLoopId);
        m_eventHandle = ::CreateEventA(NULL, FALSE, FALSE, tmp);
	}

    m_handles.push_back(m_eventHandle);
    m_objects.push_back(NULL);
    m_callbacks.push_back(NULL);
    LeaveCriticalSection(&m_criticalSection);
}

KcwEventLoop::~KcwEventLoop() {
}

void KcwEventLoop::addCallback(HANDLE hndl, eventCallback cllbck, void *callbackObject) {
    char tmp[1024];
    EnterCriticalSection(&m_criticalSection);
    sprintf(tmp, "add handle #%i in eventLoop %i value: %i", (m_handles.size() + 1), m_eventLoopId, hndl);
    OutputDebugStringA(tmp);
    m_handles.push_back(hndl);
    m_objects.push_back(callbackObject);
    m_callbacks.push_back(cllbck);
    LeaveCriticalSection(&m_criticalSection);
}

void KcwEventLoop::quit() {
    char tmp[1024];
    DWORD dwProcessId = ::GetCurrentProcessId();
	sprintf(tmp, "quit was called in process %i", dwProcessId);
	OutputDebugStringA(tmp);
    EnterCriticalSection(&m_criticalSection);
    SetEvent(m_eventHandle);
    LeaveCriticalSection(&m_criticalSection);
}

void KcwEventLoop::setRefreshInterval(int secs) {
	OutputDebugStringA("setting refresh interval");
    EnterCriticalSection(&m_criticalSection);
    m_refreshInterval = secs;
    LeaveCriticalSection(&m_criticalSection);
}

int KcwEventLoop::refreshInterval() const {
    return m_refreshInterval;
}

void KcwEventLoop::setExitEvent(HANDLE event) {
	OutputDebugStringA("setting exit event");
    EnterCriticalSection(&m_criticalSection);
    m_eventHandle = event;
    m_handles.at(0) = event;
    LeaveCriticalSection(&m_criticalSection);
}

HANDLE KcwEventLoop::exitEvent() {
    return m_eventHandle;
}

int KcwEventLoop::exec() {
    DWORD dwHandleInfo = 0, dwWaitRes = 0, dwProcessId = ::GetCurrentProcessId();
    char tmp[1024];

	std::vector<HANDLE> locHandles(m_handles);
    EnterCriticalSection(&m_criticalSection);
    const int handleSize = locHandles.size();
    for(int i = 0; i < handleSize; i++) {
        if(!GetHandleInformation(locHandles.at(i), &dwHandleInfo)) {
            sprintf(tmp, "Handle #%i is broken in process %i", i, dwProcessId);
            OutputDebugStringA(tmp);
			LeaveCriticalSection(&m_criticalSection);
			return -1;
        }
    };

	HANDLE *begin = &locHandles.front();
    while ((dwWaitRes = ::WaitForMultipleObjects(handleSize, begin, FALSE, m_refreshInterval)) != WAIT_OBJECT_0) {
//        LeaveCriticalSection(&m_criticalSection);
        if(dwWaitRes == WAIT_FAILED) {
            LPVOID lpMsgBuf;
            DWORD dw = GetLastError(); 
            FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                dw,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR) &lpMsgBuf,
                0, NULL );

//            EnterCriticalSection(&m_criticalSection);
            sprintf(tmp, "eventLoop wait failed!\npid: %i #handles: %i result: %i\n%s", 
                         dwProcessId, locHandles.size(), dwWaitRes, (CHAR*)lpMsgBuf);
            OutputDebugStringA(tmp);
            break;
        }

//        EnterCriticalSection(&m_criticalSection);
        if(dwWaitRes == WAIT_TIMEOUT) continue;

        for(unsigned i = 0; i < handleSize; i++) {
            if(dwWaitRes == WAIT_OBJECT_0 + i) {
                if(m_callbacks[i] != NULL) {
					sprintf(tmp, "calling callback for event #%i in process %i", i, dwProcessId);
					OutputDebugStringA(tmp);
					eventCallback callback = m_callbacks[i];
					void *arg = m_objects[i];
					LeaveCriticalSection(&m_criticalSection);
					callback(arg);
					EnterCriticalSection(&m_criticalSection);
				} else {
					sprintf(tmp, "calling quit for event #%i in process %i", i, dwProcessId);
					OutputDebugStringA(tmp);
					LeaveCriticalSection(&m_criticalSection);
					quit();
					EnterCriticalSection(&m_criticalSection);
				}
            }
        }
    }
	LeaveCriticalSection(&m_criticalSection);
    return 0;
}

int KcwEventLoop::getUniqueCounter() {
    // we need to check that our threadCounter has started
    // KcwSharedMemory<T>::open() and KcwSharedMemory<T>::create()
    // return 0 in case they already have been opened.
    if(s_globalEventLoopCounter.open("KcwEventLoopGlobal") != 0) {
        if(s_globalEventLoopCounter.create("KcwEventLoopGlobal", 1) != 0) {
            // in case of failure, exit the complete application
            s_globalEventLoopCounter.errorExit();
        } else {
            *s_globalEventLoopCounter = 0;
        }
    }

    // increase the counter by one, currently this is still not thread save
    char tmp[1024];
    sprintf(tmp, "opening global eventLoop number %i", *s_globalEventLoopCounter);
    OutputDebugStringA(tmp);
    return (*s_globalEventLoopCounter)++;
}
