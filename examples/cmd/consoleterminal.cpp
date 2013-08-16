#include "consoleterminal.h"

#include <kcwdebug.h>

#include "consoleinputreader.h"
#include "consoleoutputwriter.h"

ConsoleTerminal::ConsoleTerminal()
: Terminal() {
    setInputReader(new ConsoleInputReader);
    setOutputWriter(new ConsoleOutputWriter(this));
}

ConsoleTerminal::~ConsoleTerminal() {
    delete inputReader();
    delete outputWriter();
}

void ConsoleTerminal::bufferChanged() {
    KcwDebug() << __FUNCTION__;
}

void ConsoleTerminal::sizeChanged() {
    KcwDebug() << __FUNCTION__;
}

void ConsoleTerminal::cursorPositionChanged() {
    KcwDebug() << __FUNCTION__;
}
