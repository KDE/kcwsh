#ifndef qoutputwriter
#define qoutputwriter

#include "outputwriter.h"

#include <QString>
#include <QRectF>

class TerminalWidgetTerminal;

class QPainter;
namespace KcwSH {
class Terminal;
namespace QtFrontend {


class QtOutputWriter : public OutputWriter {
    public:
        QtOutputWriter(TerminalWidgetTerminal* term);
        QString getBufferText();
        void setTerminal(Terminal* t) {m_term = t;}
        void paintOutput(QPainter* p, const QRectF& r);
};
};
};
#endif /* qoutputwriter */