#ifndef consoleoutputwriter
#define consoleoutputwriter

#include <kcwthread.h>

#include "outputwriter.h"

class ConsoleTerminal;

class ConsoleOutputWriter : public KcwSH::OutputWriter {
    public:
        ConsoleOutputWriter(ConsoleTerminal* term);
        ~ConsoleOutputWriter();

        void init();

        void bufferChanged();

        virtual KCW_CALLBACK(ConsoleOutputWriter, writeData);
    private:
        void saveConsole();
        void restoreConsole();

        HANDLE m_screen;
        HANDLE m_screenSave;
};
#endif /* consoleoutputwriter */
