#include "qterminalwidget.h"

#include <QKeyEvent>
#include <QFont>
#include <QFontMetrics>
#include <QDebug>
#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QColor>
#include <QSizePolicy>
#include <QApplication>

#include "terminal.h"
#include "qinputreader.h"
#include "qoutputwriter.h"
#include "qterminalwidget_p.h"

#include <windows.h>
#include <Shlobj.h>

using namespace KcwSH;
using namespace KcwSH::QtFrontend;

std::string getDefaultCmdInterpreter() {
    CHAR szPath[MAX_PATH];

    if(SUCCEEDED(SHGetFolderPathA(NULL,
                                 CSIDL_SYSTEMX86|CSIDL_FLAG_CREATE,
                                 NULL,
                                 0,
                                 szPath)))
    {
        std::string ret(szPath);
        return ret;
    }
    return std::string();
}

void TerminalWidgetTerminal::sizeChanged() {
    qDebug() << __FUNCTION__;
    emit terminalSizeChanged();
}

void TerminalWidgetTerminal::bufferChanged() {
    qDebug() << __FUNCTION__;
    emit terminalBufferChanged();
}


/*void TerminalWidgetTerminal::quit() {
    qDebug() << "TerminalWidgetTerminal::quit";
    Terminal::quit();
}*/

void TerminalWidgetTerminal::hasQuit() {
    qDebug() << "TerminalWidgetTerminal::hasQuit!";
    Terminal::hasQuit();
    emit terminalQuit();
}

void TerminalWidgetTerminal::endTerminal() {
    qDebug() << "ending terminal!";
    Terminal::quit();
}

TerminalWidget::TerminalWidget(QWidget* parent)
: t(NULL)
, QWidget(parent) {
    setFont(QFont("Courier New"));
    startTerminal();
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
}

void TerminalWidget::startTerminal() {
    if(t != NULL) {
        disconnect(t, SIGNAL(terminalSizeChanged()), this, SLOT(resizeTerminal()));
        disconnect(t, SIGNAL(terminalBufferChanged()), this, SLOT(repaint()));
        disconnect(qApp, SIGNAL(aboutToQuit()), t, SLOT(endTerminal()));
        disconnect(t, SIGNAL(terminalQuit()), this, SLOT(startTerminal()));
        delete t;
    }
    t = new TerminalWidgetTerminal;
    connect(t, SIGNAL(terminalSizeChanged()), this, SLOT(resizeTerminal()));
    connect(t, SIGNAL(terminalBufferChanged()), this, SLOT(repaint()));
    connect(qApp, SIGNAL(aboutToQuit()), t, SLOT(endTerminal()));
    connect(t, SIGNAL(terminalQuit()), this, SLOT(startTerminal()));

    t->setCmd(getDefaultCmdInterpreter() + "\\cmd.exe");
    t->start();
}

void TerminalWidget::resizeTerminal() {
    qDebug() << "resizing terminal";
    adjustSize();
}

void TerminalWidget::keyReleaseEvent(QKeyEvent* event) {
    reinterpret_cast<QtInputReader*>(t->inputReader())->transferData(event, false);
}

void TerminalWidget::keyPressEvent(QKeyEvent* event) {
    reinterpret_cast<QtInputReader*>(t->inputReader())->transferData(event, true);
}

void TerminalWidget::paintEvent(QPaintEvent* event) {
    QPainter p(this);
    p.setPen(Qt::blue);
    p.setFont(font());
    reinterpret_cast<QtOutputWriter*>(t->outputWriter())->paintOutput(&p, rect());
}

QSize TerminalWidget::sizeHint() const {
    return minimumSize();
}

QSize TerminalWidget::minimumSizeHint() const {
    return minimumSize();
}

QSize TerminalWidget::minimumSize() const {
    COORD c = t->terminalSize();
    if(c.X == 0 || c.Y == 0) {
        c.X = 1; c.Y = 1;
    }
    QSize size(fontMetrics().width("W") * c.X, fontMetrics().height() * c.Y);
    return size;
}
