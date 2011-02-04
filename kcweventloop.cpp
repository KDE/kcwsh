#include "kcweventloop.h"

HANDLE s_appEvent = ::CreateEvent(NULL, FALSE, FALSE, "KcwEventLoop");

KcwEventLoop::KcwEventLoop(HANDLE eventHandle)
 :  m_refreshInterval(1) {
    if(eventHandle == NULL) m_eventHandle = s_appEvent;
    else m_eventHandle = eventHandle;

    m_handles.push_back(m_eventHandle);
    m_callbacks.push_back(NULL);
}

void KcwEventLoop::addCallback(HANDLE hndl, eventCallback cllbck) {
    m_handles.push_back(hndl);
    m_callbacks.push_back(cllbck);
}

void KcwEventLoop::quit() {
    SetEvent(m_eventHandle);
}

int KcwEventLoop::exec() {
    DWORD dwWaitRes = 0;

    while ((dwWaitRes = ::WaitForMultipleObjects(m_handles.size(), &m_handles[0], FALSE, m_refreshInterval)) != WAIT_OBJECT_0) {
        for(unsigned i = 1; i <= m_handles.size(); i++) {
            if(dwWaitRes == WAIT_OBJECT_0 + i) {
                if(m_callbacks[i] != NULL) m_callbacks[i]();
                else quit();
            }
        }
    }
    return 0;
}
