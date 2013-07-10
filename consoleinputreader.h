#ifndef consoleinputreader
#define consoleinputreader

#include "inputreader.h"

class ConsoleInputReader : public KcwSH::InputReader {
    public:
        ConsoleInputReader();

        void init();

        KCW_CALLBACK(ConsoleInputReader, transferData);
    private:
        HANDLE m_localConsole;
};
#endif /* consoleinputreader */