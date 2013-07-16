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
        : ow(new QtOutputWriter)
        , ir(new QtInputReader)
        , Terminal() {
            setInputReader(ir);
            setOutputWriter(ow);
        }

        ~TerminalWidgetTerminal() {
            qDebug() << "ending TerminalWidgetTerminal";
        }

//        KCW_CALLBACK(TerminalWidgetTerminal, quit)
        void sizeChanged();
        void hasQuit();

    Q_SIGNALS:
        void terminalQuit();
        void terminalSizeChanged();

    public Q_SLOTS:
        void endTerminal();
    private:
        QtOutputWriter* ow;
        QtInputReader* ir;
};

#endif /* qterminalwidgetterminal */