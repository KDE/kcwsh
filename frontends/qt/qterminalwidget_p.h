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