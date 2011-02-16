#ifndef sharedmemory_h
#define sharedmemory_h

#include <string>
#include <cstdio>
#include <windows.h>

template<typename T>
class KcwSharedMemory {
    public:

        KcwSharedMemory();
        KcwSharedMemory(const std::string& strName, DWORD dwSize, bool bCreate = true);

        inline void create(const std::string& strName, DWORD dwSize);
        inline void open(const std::string& strName);
        inline int createEx(const std::string& strName, DWORD dwSize);
        inline int openEx(const std::string& strName);
        inline void errorExit();
        inline void notify();

        inline T& operator[](size_t index) const;
        inline T* operator->() const;
        inline T& operator*() const;
        inline KcwSharedMemory& operator=(const T& val);
        HANDLE notificationEvent() const;

    private:
        std::string m_name;
        DWORD       m_size;
        HANDLE      m_sharedMemHandle;
        T*          m_sharedMem;
        HANDLE      m_notificationEvent;
};

template<typename T>
KcwSharedMemory<T>::KcwSharedMemory()
: m_name("") {
}


template<typename T>
KcwSharedMemory<T>::KcwSharedMemory(const std::string& strName, DWORD dwSize, bool bCreate)
: m_name(strName) {
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
    char* lpMsgBuf = NULL;
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
void KcwSharedMemory<T>::create(const std::string& strName, DWORD dwSize) {
    if(createEx(strName, dwSize) != 0) {
        errorExit();
    }
}

template<typename T>
int KcwSharedMemory<T>::createEx(const std::string& strName, DWORD dwSize) {
    static SECURITY_ATTRIBUTES sa;
    static SECURITY_ATTRIBUTES sa_event;

    m_name      = strName;
    m_size      = dwSize;
    
    m_notificationEvent = ::CreateEvent(&sa_event, FALSE, FALSE, (m_name + "_req_event").c_str());

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
void KcwSharedMemory<T>::open(const std::string& strName) {
    int result = openEx(strName);
    switch(result) {
        case -1:
            OutputDebugString("Error opening shared mem");
            break;
        case -2:
            OutputDebugString("Error mapping shared mem");
            break;
        default:
            break;
    }
}

template<typename T>
int KcwSharedMemory<T>::openEx(const std::string& strName) {
    m_name   = strName;
    OutputDebugString((std::string("key: ").append(m_name)).c_str());
    m_notificationEvent = ::OpenEvent(EVENT_ALL_ACCESS, FALSE, (m_name + "_req_event").c_str());
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
