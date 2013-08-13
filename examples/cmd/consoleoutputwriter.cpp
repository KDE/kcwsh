#include "consoleoutputwriter.h"

#include <kcwdebug.h>

#include "consoleterminal.h"

using namespace KcwSH;

ConsoleOutputWriter::ConsoleOutputWriter(ConsoleTerminal* term)
: OutputWriter(term) {
}

ConsoleOutputWriter::~ConsoleOutputWriter() {
    restoreConsole();
}

void ConsoleOutputWriter::writeData() {
    KcwDebug() << "writing new Data to output!";
    COORD bufferOrigin; bufferOrigin.X = 0; bufferOrigin.Y = 0;
    SMALL_RECT sr;
    sr.Top = 1; sr.Left = 0; sr.Right = m_bufferSize->X; sr.Bottom = m_bufferSize->Y + 1;
    WriteConsoleOutput(m_screen, m_output.data(), *m_bufferSize.data(), bufferOrigin, &sr);
}
void ConsoleOutputWriter::saveConsole() {
    KcwDebug() << "saving console info";
    m_screenSave = GetStdHandle(STD_OUTPUT_HANDLE);
    m_screen = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(m_screen);
}

void ConsoleOutputWriter::restoreConsole() {
    KcwDebug() << "restoring console info";
    SetConsoleActiveScreenBuffer(m_screenSave);
}

// this overwrites the implementation from KcwSH::OutputWriter, so Terminal::bufferChanged won't be called
// instead we have to call it manually
void ConsoleOutputWriter::bufferChanged() {
    writeData();
    m_term->bufferChanged();
}

void ConsoleOutputWriter::init() {
    OutputWriter::init();

    // save original state
    saveConsole();

    // now change our console size
    CONSOLE_SCREEN_BUFFER_INFOEX csbi;
    ZeroMemory(&csbi, sizeof(CONSOLE_SCREEN_BUFFER_INFOEX));
    csbi.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    KcwDebug() << "getting console info:" << GetConsoleScreenBufferInfoEx(m_screen, &csbi);
    COORD currentCoords = *m_bufferSize;
    KcwDebug() << "current size of the buffer:" << currentCoords.X << "X" << currentCoords.Y;
    int headerLines = 2;
    currentCoords.Y += headerLines;
    csbi.dwSize = currentCoords;
    SMALL_RECT sr;
    sr.Left = 0; sr.Top = headerLines; sr.Right = currentCoords.X; sr.Bottom = currentCoords.Y + headerLines;
    csbi.dwMaximumWindowSize = currentCoords;
    csbi.srWindow = sr;
    KcwDebug() << "setting Console info:" << SetConsoleScreenBufferInfoEx(m_screen, &csbi);
}
