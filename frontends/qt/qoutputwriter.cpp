#include "qoutputwriter.h"

#include <windows.h>
#include <QChar>
#include <QDebug>
#include "terminal.h"
#include <kcwdebug.h>

#include "qterminalwidget_p.h"

using namespace KcwSH::QtFrontend;

QtOutputWriter::QtOutputWriter(TerminalWidgetTerminal* term)
: OutputWriter(term) {
}

QString QtOutputWriter::getBufferText() {
    COORD size = m_term->terminalSize();
    const int maxlinenum = size.Y;
    const int maxcolnum = size.X;
    const int len = (maxcolnum * maxlinenum > 10000) ? 10000 : maxcolnum * maxlinenum;
    CHAR_INFO* buffer = new CHAR_INFO[len];
    ZeroMemory(buffer, sizeof(CHAR_INFO) * len);

    memcpy(buffer, m_output.data(), sizeof(CHAR_INFO) * len);
    QString ret = QString();
    ret.resize(len + size.Y + 1);
    WCHAR* array = new WCHAR[len+size.Y+1];
    ZeroMemory(array, sizeof(WCHAR)*(len+size.Y+1));
    WCHAR* arrayptr = &array[0];
    for(int linenum = 0; linenum < maxlinenum; linenum++) { // go through each line
        for(int colnum = 0; colnum < maxcolnum; colnum++) { // go through each column
            *arrayptr = buffer[linenum * maxcolnum + colnum].Char.UnicodeChar;
            arrayptr++;
        }
        *arrayptr = L'\n';
        arrayptr++;
    };
    *arrayptr = L'\0';
    ret = QString::fromUtf16(reinterpret_cast<ushort*>(array));
    delete[] buffer;
    delete[] array;
    return ret;
}