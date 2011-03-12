#include "kcwthreadrep.h"

#include <tlhelp32.h>


KcwThreadRep::KcwThreadRep()
 :  m_process(NULL),
    m_thread(NULL) {
}

KcwThreadRep::KcwThreadRep(HANDLE thread) { KcwThreadRep(GetCurrentProcess(), thread); }

KcwThreadRep::KcwThreadRep(HANDLE app, HANDLE thread)
 :  m_process(app),
    m_thread(thread) {
}

KcwThreadRep::KcwThreadRep(int threadid) { KcwThreadRep(GetCurrentProcess(), getHandleForThreadId(threadid)); }

KcwThreadRep::KcwThreadRep(int pid, int threadid) { KcwThreadRep(getHandleForPid(pid), getHandleForThreadId(threadid)); }

void KcwThreadRep::attachThread(HANDLE thread) {
    attachAppThread(GetCurrentProcess(), thread);
}

void KcwThreadRep::attachAppThread(HANDLE app, HANDLE thread) {
    m_process = app;
    m_thread = thread;
}

void KcwThreadRep::attachThreadId(int threadid) {
    attachAppThread(GetCurrentProcess(), getHandleForThreadId(threadid));
}

bool KcwThreadRep::resume() {
    return ResumeThread(m_thread) == 1;
}

bool KcwThreadRep::suspend() {
    return SuspendThread(m_thread) >= 0;
}

HANDLE KcwThreadRep::threadHandle() const {
    return m_thread;
}

HANDLE KcwThreadRep::processHandle() const {
    return m_process;
}

std::vector<KcwThreadRep> KcwThreadRep::threads(HANDLE app) {
    return threads(GetProcessId(app));
}

std::vector<KcwThreadRep> KcwThreadRep::threads(int pid) {
    std::vector<KcwThreadRep> ret = std::vector<KcwThreadRep>();
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if(snapshot != INVALID_HANDLE_VALUE) {
        THREADENTRY32 threadentry;
        threadentry.dwSize = sizeof(threadentry);
        if(Thread32First(snapshot, &threadentry)) {
            do {
                if(threadentry.th32OwnerProcessID == pid) {
                    ret.push_back(KcwThreadRep(pid, threadentry.th32ThreadID));
                }
            } while(Thread32Next(snapshot, &threadentry));
        }
    }
    return ret;
}

HANDLE KcwThreadRep::getHandleForPid(int pid) {
//    return OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    return OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
}

HANDLE KcwThreadRep::getHandleForThreadId(int threadid) {
    return OpenProcess(THREAD_ALL_ACCESS, FALSE, threadid);
}
