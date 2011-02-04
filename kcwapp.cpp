#include "kcwapp.h"

HANDLE KcwApp::s_appEvent = ::CreateEvent(NULL, FALSE, FALSE, "kcwapp");
KcwApp::KcwApp() : m_refreshInterval(1) {
    m_handles.push_back(s_appEvent);
    m_callbacks.push_back(NULL);
}

void KcwApp::addCallback(HANDLE hndl, eventCallback cllbck) {
    m_handles.push_back(hndl);
    m_callbacks.push_back(cllbck);
}

void KcwApp::quit() {
    SetEvent(s_appEvent);
}

int KcwApp::exec() {
    DWORD dwWaitRes = 0;

    while ((dwWaitRes = ::WaitForMultipleObjects(m_handles.size(), &m_handles[0], FALSE, m_refreshInterval)) != WAIT_OBJECT_0) {
        for(unsigned i = 1; i <= m_handles.size(); i++) {
            if(dwWaitRes == WAIT_OBJECT_0 + i) {
                m_callbacks[i]();
            }
        }
    }
    return 0;
}