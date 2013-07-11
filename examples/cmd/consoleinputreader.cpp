#include "consoleinputreader.h"

#include <kcwdebug.h>

ConsoleInputReader::ConsoleInputReader()
: m_localConsole(GetStdHandle(STD_INPUT_HANDLE))
, InputReader() {
}

void ConsoleInputReader::init() {
    InputReader::init();
    addCallback(m_localConsole, CB(transferData));
}
void ConsoleInputReader::transferData() {
    if(!this) {
        KcwDebug() << "no consoleinputreader instance!";
        return;
    }

    INPUT_RECORD *buffer = new INPUT_RECORD[m_cacheSize];
    DWORD len = 0;
    if(!ReadConsoleInput(m_localConsole, buffer, m_cacheSize, &len)) {
        KcwDebug() << "failed to read console input!";
    } else {
//         KcwDebug() << "read" << len << "records from stdin";
        memcpy(m_input.data(), buffer, len*sizeof(INPUT_RECORD));
        *m_inputSize = len;
        m_bytesWritten.notify();
    }
    delete[] buffer;
}