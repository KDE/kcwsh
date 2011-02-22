#include "kcweventloop.h"

HANDLE s_appEvent = ::CreateEvent(NULL, FALSE, FALSE, "KcwEventLoop");

KcwEventLoop::KcwEventLoop(HANDLE eventHandle)
 :  m_refreshInterval(10) {
    if(eventHandle == NULL) m_eventHandle = s_appEvent;
    else m_eventHandle = eventHandle;

    m_handles.push_back(m_eventHandle);
    m_objects.push_back(NULL);
    m_callbacks.push_back(NULL);
}

KcwEventLoop::~KcwEventLoop() {
}

void KcwEventLoop::addCallback(HANDLE hndl, eventCallback cllbck, void *callbackObject) {
    m_handles.push_back(hndl);
    m_objects.push_back(callbackObject);
    m_callbacks.push_back(cllbck);
}

void KcwEventLoop::quit() {
    char tmp[1024];
    DWORD dwProcessId = ::GetCurrentProcessId();
	sprintf(tmp, "quit was called in process %i", dwProcessId);
	OutputDebugString(tmp);
    SetEvent(m_eventHandle);
}

void KcwEventLoop::setRefreshInterval(int secs) {
    m_refreshInterval = secs;
}

int KcwEventLoop::refreshInterval() const {
    return m_refreshInterval;
}

void KcwEventLoop::setExitEvent(HANDLE event) {
    m_eventHandle = event;
    m_handles[0] = event;
}

HANDLE KcwEventLoop::exitEvent() {
    return m_eventHandle;
}

int KcwEventLoop::exec() {
    DWORD dwWaitRes = 0;
    char tmp[1024];
    DWORD dwProcessId = ::GetCurrentProcessId();

    while ((dwWaitRes = ::WaitForMultipleObjects(m_handles.size(), &m_handles[0], FALSE, m_refreshInterval)) != WAIT_OBJECT_0) {
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

            sprintf(tmp, "eventLoop wait failed!\npid: %i #handles: %i result: %i\n%s", 
                         dwProcessId, m_handles.size(), dwWaitRes, (CHAR*)lpMsgBuf);
            OutputDebugString(tmp);
            break;
        }

        if(dwWaitRes == WAIT_TIMEOUT) continue;
        
        for(unsigned i = 1; i <= m_handles.size(); i++) {
            if(dwWaitRes == WAIT_OBJECT_0 + i) {
                if(m_callbacks[i] != NULL) {
					sprintf(tmp, "calling callback for event #%i in process %i", i, dwProcessId);
					OutputDebugString(tmp);
					m_callbacks[i](m_objects[i]);
				} else {
					sprintf(tmp, "calling quit for event #%i in process %i", i, dwProcessId);
					OutputDebugString(tmp);
					quit();
				}
            }
        }
    }
    return 0;
}
