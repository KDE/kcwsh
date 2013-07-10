#ifndef kcwsh_terminal
#define kcwsh_terminal

#include <iostream>

#include <kcwthread.h>
#include <kcwprocess.h>
#include <kcwnotifier.h>

#include "kcwsh_export.h"

namespace KcwSH {

class InputReader;
class OutputWriter;

class KCWSH_EXPORT Terminal : public KcwThread {
    public:
        Terminal();
        ~Terminal();

        void setCmd(const std::string& _cmd);
        std::string cmd() const;

        void setInputReader(InputReader* reader);
        void setOutputWriter(OutputWriter* writer);
        DWORD run();
    private:
        std::wstring getModulePath(HMODULE hModule);

        InputReader *m_inputReader;
        OutputWriter *m_outputWriter;
        KcwProcess m_process;
        KcwNotifier m_exitEvent;
        KcwNotifier m_setupEvent;
};
};
#endif /* kcwsh_terminal */
