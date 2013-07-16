#include "qinputreader.h"

#include <QDebug>
#include <QKeyEvent>
#include <QString>

#include <windows.h>

using namespace KcwSH::QtFrontend;

QtInputReader::QtInputReader()
: InputReader() {
}

void QtInputReader::transferData(QKeyEvent* event, bool keyDown) {
    INPUT_RECORD ir;
    ZeroMemory(&ir, sizeof(INPUT_RECORD));
    ir.EventType = KEY_EVENT;
    ir.Event.KeyEvent.bKeyDown = keyDown;
    ir.Event.KeyEvent.wRepeatCount = event->count();
    ir.Event.KeyEvent.wVirtualKeyCode = event->nativeVirtualKey();
    ir.Event.KeyEvent.wVirtualScanCode = event->nativeScanCode();
    ir.Event.KeyEvent.uChar.UnicodeChar = static_cast<WCHAR>(event->text().utf16()[0]);
    ir.Event.KeyEvent.dwControlKeyState = event->nativeModifiers();

    memcpy(m_input.data(), &ir, sizeof(INPUT_RECORD));
    *m_inputSize = 1;
    m_bytesWritten.notify();
}