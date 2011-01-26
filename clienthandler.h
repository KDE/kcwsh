#ifndef clienthandler_h
#define clienthandler_h

#include <string>

#include <windows.h>

class ClientHandler {
    public:
        ClientHandler(std::string procname);
        
        bool start();
        bool inject();
        bool inject2();
    private:
        PROCESS_INFORMATION m_procInfo;
        std::string m_procName;
};

#endif /* clienthandler_h */
