#ifndef kcwsh_terminal
#define kcwsh_terminal

#include <iostream>

#include "kcwsh_export.h"

namespace KcwSH {
class Terminal  {
    public:
        Terminal();

        void setCmd(const std::wstring& cmd);
        std::wstring cmd() const;

        bool open();
    private:
        m_cmd;
};
};
#endif /* kcwsh_terminal */