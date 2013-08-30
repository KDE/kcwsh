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
        InputReader* inputReader() const;
        OutputWriter* outputWriter() const;

        bool isSetup() const;

        /**
         * this function closes down the complete terminal and shuts down the terminal thread
         */
        void quit();

        virtual KCW_CALLBACK(Terminal, sizeChanged);
        virtual KCW_CALLBACK(Terminal, bufferChanged);
        virtual KCW_CALLBACK(Terminal, cursorPositionChanged);

        /**
         * this callback will be called as soon as the terminal process has finished;
         * a finished terminal thread will automatically end this Terminal's thread
         * it won't be called in case you call quit directly
         */
        virtual KCW_CALLBACK(Terminal, hasQuit);

        virtual KCW_CALLBACK(Terminal, aboutToQuit);

        virtual KCW_CALLBACK(Terminal, activate);
        virtual KCW_CALLBACK(Terminal, deactivate);

        void setActive(bool t);
        bool active() const;

        void setTerminalSize(COORD c);
        COORD terminalSize() const;

        DWORD run();

        void sendText(const std::wstring& t);
        void sendCommand(const std::wstring& c);

    protected:
        Terminal(InputReader* ir, OutputWriter* ow);

    private:
        std::wstring getModulePath(HMODULE hModule);
        KCW_CALLBACK(Terminal, inputThreadDetached);
        KCW_CALLBACK(Terminal, outputThreadDetached);

        bool m_setup;
        bool m_active;

        HANDLE m_inputWriter;
        HANDLE m_outputReader;

        InputReader *m_inputReader;
        OutputWriter *m_outputWriter;
        KcwProcess m_process;
        KcwNotifier m_exitEvent;
        KcwNotifier m_setupEvent;
};
};
#endif /* kcwsh_terminal */
