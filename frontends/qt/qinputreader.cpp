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
    sendKeyboardEvents(&ir, 1);
}