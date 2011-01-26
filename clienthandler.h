#ifndef clienthandler_h
#define clienthandler_h

#include <string>

#include <windows.h>

class ClientHandler {
    public:
        ClientHandler(std::string procname);
        
        bool start(HANDLE _stdin = 0, HANDLE _stdout = 0, HANDLE _stderr = 0);
        bool inject();
    private:
        PROCESS_INFORMATION m_procInfo;
        std::string m_procName;
};

#endif /* clienthandler_h */
