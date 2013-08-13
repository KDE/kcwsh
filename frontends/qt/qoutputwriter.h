#ifndef qoutputwriter
#define qoutputwriter

#include "outputwriter.h"

#include <QString>
#include <QObject>

class TerminalWidgetTerminal;

namespace KcwSH {
class Terminal;
namespace QtFrontend {


class QtOutputWriter : public QObject, public OutputWriter {
    Q_OBJECT
    public:
        QtOutputWriter(TerminalWidgetTerminal* term);
        QString getBufferText();
        void setTerminal(Terminal* t) {m_term = t;}
//         KCW_CALLBACK(QtOutputWriter, bufferChanged);

        void start();
        void pause();
};
};
};
#endif /* qoutputwriter */