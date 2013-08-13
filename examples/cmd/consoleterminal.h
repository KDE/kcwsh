#ifndef consoleterminal
#define consoleterminal

#include "terminal.h"

class ConsoleTerminal : public KcwSH::Terminal {
    public:
        ConsoleTerminal();
        ~ConsoleTerminal();

        void bufferChanged();
        void sizeChanged();
};

#endif /* consoleterminal */