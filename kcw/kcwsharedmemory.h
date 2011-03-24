#ifndef sharedmemory_h
#define sharedmemory_h

#include <string>
#include <cstdio>
#include <windows.h>

/**
* @author Patrick Spendrin
* @date 2011
* @brief This class is used to abstract a shared memory object
* @details Shared memory can be used for interprocess communication. For this purpose
* a KcwSharedMemory object is created on both sides, on one of them create() is called,
* on the other one you call open(). After that, you can use the memory objects on both
* sides similar to a pointer to a normal object.
*/
template<typename T>
class KcwSharedMemory {
    public:
        KcwSharedMemory();
        KcwSharedMemory(const std::wstring& strName, int size = 1, bool bCreate = true);

        /**@{*/
        /**
        * create a new shared memory space with the name @p strName and @p size. In case that a shared memory
        * space is already opened in this object, do not create a new one but simply return 0.
        * @return 0 in case a shared memory could be added and a value lower than 0 in case something
        * went wrong.
        */
        inline int create(const std::wstring& strName, int size = 1);

        /**
        * open an existing shared memory space with the name @p strName. In case that a shared memory
        * space is already opened in this object, do not create a new one but simply return 0.
        * @return 0 in case a shared memory could be added and a value lower than 0 in case something
        * went wrong.
        */
        inline int open(const std::wstring& strName);
        /**@}*/

        /**
        * An internal convenience function to output the last error that occured and exit the application.
        */
        inline void errorExit();


        inline T& operator[](size_t index) const;
        inline T* operator->() const;
        inline T& operator*() const;
        inline KcwSharedMemory& operator=(const T& val);

        /**
        * @return a pointer to the shared memory segment.
        */
        inline T* data();

        /**@{*/
        /**
        * notify the other sides of the shared memory segment. This function is normally used to signal
        * that the contents of the shared memory has been changed.
        */
        inline void notify();

        /**
        * @return the event which is used to notify() the other side.
        */
        HANDLE notificationEvent() const;
        /**@}*/

        /**
        * @return the size of this shared memory segment.
        */
        int size() const;

    private:
        std::wstring m_name;
        int         m_size;
        HANDLE      m_sharedMemHandle;
        T*          m_sharedMem;
        HANDLE      m_notificationEvent;
};

/**
* constructs a default object of Type T. The object cannot be used before calling
* either open() or create().
*/
template<typename T>
KcwSharedMemory<T>::KcwSharedMemory()
: m_name(L""),
  m_size(0),
  m_sharedMemHandle(NULL) {
}


/**
* constructs an object with the name @p strName; If you want to just open the shared memory
* add @p bCreate.
* This is only available for convenience, it is equivalent to construction of a default and
* KcwSharedMemory object and calling open() or create() afterwards.
*/
template<typename T>
KcwSharedMemory<T>::KcwSharedMemory(const std::wstring& strName, int size, bool bCreate)
: m_name(strName),
  m_size(size),
  m_sharedMemHandle(NULL) {
    if (bCreate)
    {
        create(strName, size);
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
int KcwSharedMemory<T>::create(const std::wstring& strName, int size) {
    static SECURITY_ATTRIBUTES sa;
    static SECURITY_ATTRIBUTES sa_event;

    m_name      = strName;
    m_size      = size;

    // if the file handle already is set, expect it to be set correctly and don't reopen it
    if(m_sharedMemHandle != NULL) return 0;

    m_notificationEvent = ::CreateEvent(&sa_event, FALSE, FALSE, (m_name + L"_req_event").c_str());

    m_sharedMemHandle = ::CreateFileMapping(INVALID_HANDLE_VALUE,
                                            &sa,
                                            PAGE_EXECUTE_READWRITE,
                                            0,
                                            m_size * sizeof(T) + sizeof(m_size),
                                            m_name.c_str());

    if(m_sharedMemHandle == NULL) {
        return -1;
    }

    m_sharedMem = static_cast<T*>(::MapViewOfFile(m_sharedMemHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0));
    if(m_sharedMem == NULL) {
        return -2;
    }

    ::ZeroMemory(m_sharedMem, m_size * sizeof(T) + sizeof(m_size));
    memcpy(m_sharedMem, &m_size, sizeof(m_size));
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
    memcpy(&m_size, m_sharedMem, sizeof(m_size));
    return 0;
}

template<typename T>
T* KcwSharedMemory<T>::operator->() const {
    return m_sharedMem + sizeof(m_size);
}

template<typename T>
T& KcwSharedMemory<T>::operator*() const {
    return *(m_sharedMem + sizeof(m_size));
}

template<typename T>
KcwSharedMemory<T>& KcwSharedMemory<T>::operator=(const T& val) {
    *(m_sharedMem + sizeof(m_size)) = val;
    return *this;
}

template<typename T>
T& KcwSharedMemory<T>::operator[](size_t index) const {
    return *(m_sharedMem + sizeof(m_size) + index * sizeof(T));
}

template<typename T>
T* KcwSharedMemory<T>::data() {
    return m_sharedMem + sizeof(m_size);
}

template<typename T>
int KcwSharedMemory<T>::size() const {
    return m_size;
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
