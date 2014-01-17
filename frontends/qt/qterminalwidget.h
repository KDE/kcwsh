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
#ifndef qterminalwidget
#define qterminalwidget

#include "kcwsh_export.h"

#include <QWidget>
#include <QString>

class TerminalWidgetTerminal;

namespace KcwSH {
namespace QtFrontend {

class KCWSHQT_EXPORT TerminalWidget : public QWidget {
        Q_OBJECT
    public:
        TerminalWidget(const QString& shell, QWidget* parent = 0);
        TerminalWidget(QWidget* parent = 0);
        QSize minimumSizeHint() const;
        QSize minimumSize() const;
        QSize sizeHint() const;

        QString title() const;
        void setTitle(const QString& t);

    Q_SIGNALS:
        void titleChanged(QString t);

    private Q_SLOTS:
        void resizeTerminal();
        void startTerminal();
        void showContextMenu(const QPoint& p);
//         void aboutToQuit();
    private:
        void keyReleaseEvent(QKeyEvent* event);
        void keyPressEvent(QKeyEvent* event);
        void paintEvent(QPaintEvent* event);
        void resizeEvent(QResizeEvent* event);
        QString m_shell;
        TerminalWidgetTerminal* t;
};
};
};
#endif /* qterminalwidget */
