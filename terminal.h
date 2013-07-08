#ifndef kcwsh_terminal
#define kcwsh_terminal

#include <iostream>

#include "kcwsh_export.h"

namespace KcwSH {
class KCWSH_EXPORT Terminal  {
    public:
        Terminal();

        void setCmd(const std::wstring& cmd);
        std::wstring cmd() const;

        bool open();
    private:
        std::wstring m_cmd;
};
};
#endif /* kcwsh_terminal */