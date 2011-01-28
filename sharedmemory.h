#ifndef sharedmemory_h
#define sharedmemory_h

#include <iostream>
#include <cstdio>
#include <windows.h>

template<typename T>
class SharedMemory
{
    public:

        SharedMemory();
        SharedMemory(const std::string& strName, DWORD dwSize, bool bCreate = true);

        void create(const std::string& strName, DWORD dwSize);
        void open(const std::string& strName);

        inline T& operator[](size_t index) const;
        inline T* operator->() const;
        inline T& operator*() const;
        inline SharedMemory& operator=(const T& val);

    private:
        std::string m_name;
        DWORD       m_size;
        HANDLE      m_sharedMemHandle;
        T*          m_sharedMem;
};

template<typename T>
SharedMemory<T>::SharedMemory()
: m_name("")
{
}


template<typename T>
SharedMemory<T>::SharedMemory(const std::string& strName, DWORD dwSize, bool bCreate)
: m_name(strName)
{
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
void SharedMemory<T>::create(const std::string& strName, DWORD dwSize)
{
    SECURITY_ATTRIBUTES sa;
    char tmp[1024];

    m_name      = strName;
    m_size      = dwSize;

    m_sharedMemHandle = ::CreateFileMapping(INVALID_HANDLE_VALUE,
                                            &sa,
                                            PAGE_READWRITE,
                                            0,
                                            m_size * sizeof(T),
                                            m_name.c_str());


    m_sharedMem = static_cast<T*>(::MapViewOfFile(m_sharedMemHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0));

    sprintf(tmp, "sharedMem: %p", m_sharedMem);
    OutputDebugString(tmp);
    ::ZeroMemory(m_sharedMem, m_size * sizeof(T));
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

template<typename T>
void SharedMemory<T>::open(const std::string& strName)
{
    m_name   = strName;

    m_sharedMemHandle = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, m_name.c_str());

    if (!m_sharedMemHandle || (m_sharedMemHandle == INVALID_HANDLE_VALUE)) {
        OutputDebugString("Error opening shared mem");
    }

    m_sharedMem = static_cast<T*>(::MapViewOfFile(m_sharedMemHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0));

    if (!m_sharedMem) {
        OutputDebugString("Error mapping shared mem");
    }
}

template<typename T>
T& SharedMemory<T>::operator[](size_t index) const
{
    return *(m_sharedMem + index);
}

template<typename T>
T* SharedMemory<T>::operator->() const
{
    return m_sharedMem;
}

template<typename T>
T& SharedMemory<T>::operator*() const
{
    return *m_sharedMem;
}

template<typename T>
SharedMemory<T>& SharedMemory<T>::operator=(const T& val)
{
    *m_sharedMem = val;
    return *this;
}


#endif /* sharedmemory_h */
