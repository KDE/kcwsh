#ifndef sharedmemory_h
#define sharedmemory_h

#include <string>
#include <cstdio>
#include <windows.h>

template<typename T>
class KcwSharedMemory {
    public:

        KcwSharedMemory();
        KcwSharedMemory(const std::wstring& strName, DWORD dwSize, bool bCreate = true);

        inline int create(const std::wstring& strName, DWORD dwSize);
        inline int open(const std::wstring& strName);
        inline void errorExit();
        inline void notify();

        inline T& operator[](size_t index) const;
        inline T* operator->() const;
        inline T& operator*() const;
        inline KcwSharedMemory& operator=(const T& val);
        HANDLE notificationEvent() const;

    private:
        std::wstring m_name;
        DWORD       m_size;
        HANDLE      m_sharedMemHandle;
        T*          m_sharedMem;
        HANDLE      m_notificationEvent;
};

template<typename T>
KcwSharedMemory<T>::KcwSharedMemory()
: m_name(L""),
  m_sharedMemHandle(NULL) {
}


template<typename T>
KcwSharedMemory<T>::KcwSharedMemory(const std::wstring& strName, DWORD dwSize, bool bCreate)
: m_name(strName),
  m_sharedMemHandle(NULL) {
    if (bCreate)
    {
        create(strName, dwSize);
    }
    else
    {
        open(strName);
    }
}

template<typename T>
void KcwSharedMemory<T>::errorExit() {
    WCHAR* lpMsgBuf = NULL;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        lpMsgBuf,
        0, NULL );

    OutputDebugString(lpMsgBuf);
    LocalFree(lpMsgBuf);
    ExitProcess(dw);
}

template<typename T>
int KcwSharedMemory<T>::create(const std::wstring& strName, DWORD dwSize) {
    static SECURITY_ATTRIBUTES sa;
    static SECURITY_ATTRIBUTES sa_event;

    m_name      = strName;
    m_size      = dwSize;

    // if the file handle already is set, expect it to be set correctly and don't reopen it
    if(m_sharedMemHandle != NULL) return 0;

    m_notificationEvent = ::CreateEvent(&sa_event, FALSE, FALSE, (m_name + L"_req_event").c_str());

    m_sharedMemHandle = ::CreateFileMapping(INVALID_HANDLE_VALUE,
                                            &sa,
                                            PAGE_EXECUTE_READWRITE,
                                            0,
                                            m_size * sizeof(T),
                                            m_name.c_str());

    if(m_sharedMemHandle == NULL) {
        return -1;
    }

    m_sharedMem = static_cast<T*>(::MapViewOfFile(m_sharedMemHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0));
    if(m_sharedMem == NULL) {
        return -2;
    }

    ::ZeroMemory(m_sharedMem, m_size * sizeof(T));
    return 0;
}

template<typename T>
int KcwSharedMemory<T>::open(const std::wstring& strName) {
    m_name   = strName;

    // if the file handle already is set, expect it to be set correctly and don't reopen it
    if(m_sharedMemHandle != NULL) return 0;

//    OutputDebugStringA((std::wstring("key: ").append(m_name)).c_str());

    m_notificationEvent = ::OpenEvent(EVENT_ALL_ACCESS, FALSE, (m_name + L"_req_event").c_str());
    m_sharedMemHandle = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, m_name.c_str());

    if (!m_sharedMemHandle || (m_sharedMemHandle == INVALID_HANDLE_VALUE)) {
        return -1;
    }

    m_sharedMem = static_cast<T*>(::MapViewOfFile(m_sharedMemHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0));

    if (!m_sharedMem) {
        return -2;
    }
    return 0;
}

template<typename T>
T& KcwSharedMemory<T>::operator[](size_t index) const {
    return *(m_sharedMem + index);
}

template<typename T>
T* KcwSharedMemory<T>::operator->() const {
    return m_sharedMem;
}

template<typename T>
T& KcwSharedMemory<T>::operator*() const {
    return *m_sharedMem;
}

template<typename T>
KcwSharedMemory<T>& KcwSharedMemory<T>::operator=(const T& val) {
    *m_sharedMem = val;
    return *this;
}

template<typename T>
void KcwSharedMemory<T>::notify() {
    SetEvent(m_notificationEvent);
}

template<typename T>
HANDLE KcwSharedMemory<T>::notificationEvent() const {
    return m_notificationEvent;
}

#endif /* sharedmemory_h */
