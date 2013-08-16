#include "qoutputwriter.h"

#include <windows.h>
#include <QBrush>
#include <QChar>
#include <QColor>
#include <QDebug>
#include <QFontMetrics>
#include <QPainter>
#include <QPen>

#include "terminal.h"
#include <kcwdebug.h>

#include "qterminalwidget_p.h"

using namespace KcwSH::QtFrontend;


QPen penForAttribute(int attr)
{
    int shortAttributes = attr & 0xf;
    QColor colorArray[16];
    colorArray[0x0] = QColor(0x00, 0x00, 0x00);
    colorArray[0x1] = QColor(0x00, 0x00, 0x80);
    colorArray[0x2] = QColor(0x00, 0x80, 0x00);
    colorArray[0x3] = QColor(0x00, 0x80, 0x80);
    colorArray[0x4] = QColor(0x80, 0x00, 0x00);
    colorArray[0x5] = QColor(0x80, 0x00, 0x80);
    colorArray[0x6] = QColor(0x80, 0x80, 0x00);
    colorArray[0x7] = QColor(0xc0, 0xc0, 0xc0);
    colorArray[0x8] = QColor(0x80, 0x80, 0x80);
    colorArray[0x9] = QColor(0x00, 0x00, 0xff);
    colorArray[0xa] = QColor(0x00, 0xff, 0x00);
    colorArray[0xb] = QColor(0x00, 0xff, 0xff);
    colorArray[0xc] = QColor(0xff, 0x00, 0x00);
    colorArray[0xd] = QColor(0xff, 0x00, 0xff);
    colorArray[0xe] = QColor(0xff, 0xff, 0x00);
    colorArray[0xf] = QColor(0xff, 0xff, 0xff);

    return QPen(colorArray[shortAttributes]);
}

QBrush brushForAttribute(int attr)
{
    int shortAttributes = (attr & 0xf0) >> 4;
    QColor colorArray[16];
    colorArray[0x0] = QColor(0x00, 0x00, 0x00);
    colorArray[0x1] = QColor(0x00, 0x00, 0x80);
    colorArray[0x2] = QColor(0x00, 0x80, 0x00);
    colorArray[0x3] = QColor(0x00, 0x80, 0x80);
    colorArray[0x4] = QColor(0x80, 0x00, 0x00);
    colorArray[0x5] = QColor(0x80, 0x00, 0x80);
    colorArray[0x6] = QColor(0x80, 0x80, 0x00);
    colorArray[0x7] = QColor(0xc0, 0xc0, 0xc0);
    colorArray[0x8] = QColor(0x80, 0x80, 0x80);
    colorArray[0x9] = QColor(0x00, 0x00, 0xff);
    colorArray[0xa] = QColor(0x00, 0xff, 0x00);
    colorArray[0xb] = QColor(0x00, 0xff, 0xff);
    colorArray[0xc] = QColor(0xff, 0x00, 0x00);
    colorArray[0xd] = QColor(0xff, 0x00, 0xff);
    colorArray[0xe] = QColor(0xff, 0xff, 0x00);
    colorArray[0xf] = QColor(0xff, 0xff, 0xff);

    return QBrush(colorArray[shortAttributes]);
}

QtOutputWriter::QtOutputWriter(TerminalWidgetTerminal* term)
: OutputWriter(term) {
}

void QtOutputWriter::paintOutput(QPainter* p, const QRectF& r) {
    if(m_term->isSetup()) {
        QFontMetrics fm(p->font());
        const int fontHeight = fm.height();
        const int fontWidth = fm.width("W");

        COORD size = m_term->terminalSize();
        const int maxlinenum = size.Y;
        const int maxcolnum = size.X;
        const int len = (maxcolnum * maxlinenum > 10000) ? 10000 : maxcolnum * maxlinenum;

        CHAR_INFO* buffer = new CHAR_INFO[len];
        ZeroMemory(buffer, sizeof(CHAR_INFO) * len);
        memcpy(buffer, m_output.data(), sizeof(CHAR_INFO) * len);

        WCHAR* array = new WCHAR[maxcolnum + 1];
        const WCHAR* beginLine = &array[0];
        ZeroMemory(array, sizeof(WCHAR)*(maxcolnum + 1));
        QString tempText(maxcolnum, 0);
        int oldattr = 0;
        p->setBackgroundMode(Qt::OpaqueMode);
        for(int linenum = 0; linenum < maxlinenum; linenum++) { // go through each line
            WCHAR* arrayptr = &array[0];
            WCHAR* curLine = arrayptr;
            oldattr = buffer[linenum * maxcolnum].Attributes;
            for(int colnum = 0; colnum < maxcolnum; colnum++) { // go through each column
                const int curattr = buffer[linenum * maxcolnum + colnum].Attributes;
                if(curattr != oldattr) { // in case the attribute changes, we need to adjust our QPen style and draw the text
                    *arrayptr = L'\0';
                    tempText = QString::fromUtf16(reinterpret_cast<ushort*>(curLine));
                    p->setPen(penForAttribute(oldattr));
                    p->setBackground(brushForAttribute(oldattr));
                    p->drawText(r.x() + (curLine - beginLine) * fontWidth, r.y() + linenum * fontHeight, 
                                r.x() + colnum * fontWidth,                r.y() + (linenum + 1) * fontHeight, 
                                Qt::AlignLeft | Qt::AlignTop, tempText);
                    curLine = arrayptr;
                    oldattr = curattr;
                }
                *arrayptr = buffer[linenum * maxcolnum + colnum].Char.UnicodeChar;
                arrayptr++;
            }
            *arrayptr = L'\0';
            tempText = QString::fromUtf16(reinterpret_cast<ushort*>(curLine));
            p->setPen(penForAttribute(oldattr));
            p->setBackground(brushForAttribute(oldattr));
            p->drawText(r.x() + (curLine - beginLine) * fontWidth, r.y() + linenum * fontHeight, 
                        r.width(),                                 r.y() + (linenum + 1) * fontHeight, 
                        Qt::AlignLeft | Qt::AlignTop, tempText);
        };
        delete[] buffer;
        delete[] array;
    }
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