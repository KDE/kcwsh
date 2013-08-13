#include "qoutputwriter.h"

#include <windows.h>
#include <QChar>
#include <QDebug>
#include "terminal.h"

#include "qterminalwidget_p.h"

using namespace KcwSH::QtFrontend;

QtOutputWriter::QtOutputWriter(TerminalWidgetTerminal* term)
: OutputWriter(term) {
}

void QtOutputWriter::start() {
    // do nothing in here
}

void QtOutputWriter::pause() {
    // do nothing in here too
}

QString QtOutputWriter::getBufferText() {
    COORD size = m_term->terminalSize();
    const int len = (size.X * size.Y > 10000) ? 10000 : size.X * size.Y;
    qDebug() << "len:" << len;
    CHAR_INFO* buffer = new CHAR_INFO[len];
    ZeroMemory(buffer, sizeof(CHAR_INFO) * len);
//     memcpy(buffer, m_output.data(), sizeof(CHAR_INFO) * len);
    QString ret = QString();
//     for(int i = 0; i < size.Y; i++) { // go through each line
//         for(int j = 0; j < size.X; j++) { // go through each column
//             ret.append(buffer[j * size.X + i].Char.UnicodeChar);
//         }
// //        ret.append('\n');
//     };
//     ret.append('\0');
//     delete[] buffer;
    return ret;
}