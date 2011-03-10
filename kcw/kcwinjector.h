#ifndef kcwinjector_h
#define kcwinjector_h

/**
* @author Patrick Spendrin
* @date 2011
* @brief This class is used to abstract a dll injection
*/

#include <windows.h>
#include <string>

class KcwInjector {
    public:
        /**
        * This function sets the destination process and its main thread.
        */
        void setDestinationProcess(HANDLE proc, HANDLE thread);

        /**
        * This functions sets the path to the dll that should be injected
        */
        void setInjectionDll(std::string dllPath);

        /**
        * this function injects the dll
        */
        bool inject();
    private:
        HANDLE m_destProcess;
        HANDLE m_destThread;
        std::string m_dllPath;

};
#endif /* kcwinjector_h */
