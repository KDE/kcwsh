#ifndef consoleoutputwriter
#define consoleoutputwriter

#include <kcwthread.h>

#include "outputwriter.h"

class ConsoleOutputWriter : public KcwSH::OutputWriter {
    public:
        ConsoleOutputWriter();
        ~ConsoleOutputWriter();

        void init();

        virtual KCW_CALLBACK(ConsoleOutputWriter, writeData);
    private:
        void saveConsole();
        void restoreConsole();

        HANDLE m_screen;
        HANDLE m_screenSave;
};
#endif /* consoleoutputwriter */
