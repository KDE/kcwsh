#ifndef kcwsh_terminal
#define kcwsh_terminal

#include <iostream>

#include <kcwthread.h>
#include <kcwprocess.h>
#include <kcwnotifier.h>

#include "kcwsh_export.h"
#include "inputreader.h"

namespace KcwSH {
class KCWSH_EXPORT Terminal : public KcwThread {
    public:
        Terminal();
        ~Terminal();

        void setCmd(const std::string& _cmd);
        std::string cmd() const;

        DWORD run();
    private:
        std::wstring getModulePath(HMODULE hModule);

        InputReader m_inputReader;
        KcwProcess m_process;
        KcwNotifier m_exitEvent;
};
};
#endif /* kcwsh_terminal */
