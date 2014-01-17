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


QColor colorForAttribute(int attr)
{
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

    return colorArray[attr];
}

QtOutputWriter::QtOutputWriter(TerminalWidgetTerminal* term)
: OutputWriter(term) {
}

void QtOutputWriter::paintOutput(QPainter* p, const QRectF& r) {
    if(m_term->isSetup()) {
        QFontMetrics fm(p->font());
        const int fontHeight = fm.height();
        const int fontWidth = fm.width("W");

        if(WaitForSingleObject(m_mutex, 1000) != WAIT_OBJECT_0) {
            KcwDebug() << __FUNCTION__ << "failed!";
            return;
        }
        COORD size = m_term->terminalSize();
        const int maxlinenum = size.Y;
        const int maxcolnum = size.X;
        const int len = maxcolnum * maxlinenum;

        CHAR_INFO* buffer = new CHAR_INFO[len];
        ZeroMemory(buffer, sizeof(CHAR_INFO) * len);
        memcpy(buffer, m_output.data(), sizeof(CHAR_INFO) * len);
        ReleaseMutex(m_mutex);

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
                if(curattr & COMMON_LVB_UNDERSCORE) qDebug() << __FUNCTION__ << "(" << linenum << ", " << colnum << ")";
                if(curattr != oldattr) { // in case the attribute changes, we need to adjust our QPen style and draw the text
                    *arrayptr = L'\0';
                    tempText = QString::fromUtf16(reinterpret_cast<ushort*>(curLine));
                    p->setPen(colorForAttribute(oldattr & 0xf));
                    p->setBackground(QBrush(colorForAttribute((oldattr >> 4) & 0xf)));
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
            p->setPen(colorForAttribute(oldattr & 0xf));
            p->setBackground(QBrush(colorForAttribute((oldattr >> 4) & 0xf)));
            p->drawText(r.x() + (curLine - beginLine) * fontWidth, r.y() + linenum * fontHeight, 
                        r.width(),                                 r.y() + (linenum + 1) * fontHeight, 
                        Qt::AlignLeft | Qt::AlignTop, tempText);
        };

        // overwrite the character where the cursor is and simply invert the character colors
        const COORD cp = *m_cursorPosition;
        WCHAR s[2]; s[0] = buffer[cp.Y * maxcolnum + cp.X].Char.UnicodeChar; s[1] = 0;
        int attr = buffer[cp.Y * maxcolnum + cp.X].Attributes;
        tempText = QString::fromUtf16(reinterpret_cast<ushort*>(s));
        p->setPen(colorForAttribute((attr >> 4) & 0xf));
        p->setBackground(QBrush(colorForAttribute(attr & 0xf)));
        p->drawText(r.x() + cp.X * fontWidth      , r.y() + cp.Y * fontHeight,
                    r.x() + (cp.X + 1) * fontWidth, r.y() + (cp.Y + 1) * fontHeight,
                    Qt::AlignLeft | Qt::AlignTop, tempText);
        delete[] buffer;
        delete[] array;
    }
}

QString QtOutputWriter::getBufferText() {
    COORD size = m_term->terminalSize();
    const int maxlinenum = size.Y;
    const int maxcolnum = size.X;
    const int len = maxcolnum * maxlinenum;
    CHAR_INFO* buffer = new CHAR_INFO[len];
    ZeroMemory(buffer, sizeof(CHAR_INFO) * len);

    if(WaitForSingleObject(m_mutex, 1000) != WAIT_OBJECT_0) {
        qDebug() << __FUNCTION__ << "failed!";
    }
    memcpy(buffer, m_output.data(), sizeof(CHAR_INFO) * len);
    ReleaseMutex(m_mutex);
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