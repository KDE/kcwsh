#include "consoleoutputwriter.h"

#include <kcwdebug.h>

using namespace KcwSH;

ConsoleOutputWriter::ConsoleOutputWriter()
: OutputWriter() {
}

void ConsoleOutputWriter::writeData() {
    KcwDebug() << "writing new Data to output!";
}

void ConsoleOutputWriter::init() {
    OutputWriter::init();
    addCallback(m_bufferChanged.handle(), CB(ConsoleOutputWriter::writeData));
}