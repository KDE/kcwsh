#ifndef consoleoutputwriter
#define consoleoutputwriter

#include <kcwthread.h>

#include "outputwriter.h"

class ConsoleOutputWriter : public KcwSH::OutputWriter {
    public:
        ConsoleOutputWriter();

        void init();

        virtual KCW_CALLBACK(ConsoleOutputWriter, writeData);
    private:
};
#endif /* consoleoutputwriter */