/* Copyright 2013-2014  Patrick Spendrin <ps_ml@gmx.de>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
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

void ConsoleOutputWriter::cursorPositionChanged() {
    COORD pos = *m_cursorPosition.data();
    pos.Y += 1;
    SetConsoleCursorPosition(m_screen, pos);
    m_term->cursorPositionChanged();
}

void ConsoleOutputWriter::init() {
    OutputWriter::init();

    // save original state
    saveConsole();

    // now change our console size
//     CONSOLE_SCREEN_BUFFER_INFO csbi;
//     ZeroMemory(&csbi, sizeof(CONSOLE_SCREEN_BUFFER_INFO));
//     csbi.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFO);
//     KcwDebug() << "getting console info:" << GetConsoleScreenBufferInfo(m_screen, &csbi);
    COORD currentCoords = *m_bufferSize;
    KcwDebug() << "current size of the buffer:" << currentCoords.X << "X" << currentCoords.Y;
    int headerLines = 2;
    currentCoords.Y += headerLines;
    SetConsoleScreenBufferSize(m_screen, currentCoords);
//     csbi.dwSize = currentCoords;
    SMALL_RECT sr;
    sr.Left = 0; sr.Top = headerLines; sr.Right = currentCoords.X; sr.Bottom = currentCoords.Y + headerLines;
//     csbi.dwMaximumWindowSize = currentCoords;
    SetConsoleWindowInfo(m_screen, TRUE, &sr);
//     csbi.srWindow = sr;
//     KcwDebug() << "setting Console info:" << SetConsoleScreenBufferInfoEx(m_screen, &csbi);
}
