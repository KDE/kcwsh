#ifndef qoutputwriter
#define qoutputwriter

#include "outputwriter.h"

#include <QString>

class TerminalWidgetTerminal;

namespace KcwSH {
class Terminal;
namespace QtFrontend {


class QtOutputWriter : public OutputWriter {
    public:
        QtOutputWriter(TerminalWidgetTerminal* term);
        QString getBufferText();
        void setTerminal(Terminal* t) {m_term = t;}
};
};
};
#endif /* qoutputwriter */