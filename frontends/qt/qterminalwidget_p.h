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
#ifndef qterminalwidgetterminal
#define qterminalwidgetterminal

#include "terminal.h"

#include <QDebug>
#include <QObject>

#include "qoutputwriter.h"
#include "qinputreader.h"

using namespace KcwSH;
using namespace KcwSH::QtFrontend;

class TerminalWidgetTerminal : public QObject, public Terminal {
    Q_OBJECT
    public:
        KCW_CALLBACK(TerminalWidgetTerminal, termEnded)

        TerminalWidgetTerminal()
        : ow(NULL)
        , ir(new QtInputReader)
        , Terminal() {
            ow = new QtOutputWriter(this);
            setInputReader(ir);
            setOutputWriter(ow);
        }

        ~TerminalWidgetTerminal() {
            endTerminal();
            delete ow;
            delete ir;
        }

//        KCW_CALLBACK(TerminalWidgetTerminal, quit)
        void sizeChanged();
        void bufferChanged();
        void cursorPositionChanged();
        void hasQuit();
        void titleChanged();

    Q_SIGNALS:
        void terminalQuit();
        void terminalSizeChanged();
        void terminalBufferChanged();
        void terminalCursorPositionChanged();
        void terminalTitleChanged(QString t);

    public Q_SLOTS:
        void endTerminal();
    private:
        QtOutputWriter* ow;
        QtInputReader* ir;
};

#endif /* qterminalwidgetterminal */