#include "qterminalwidget.h"

#include <QKeyEvent>
#include <QFont>
#include <QFontMetrics>
#include <QDebug>
#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QColor>
#include <QMenu>
#include <QPoint>
#include <QSizePolicy>
#include <QClipboard>
#include <QMimeData>
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
                                 CSIDL_SYSTEM|CSIDL_FLAG_CREATE,
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
//     qDebug() << __FUNCTION__;
    emit terminalSizeChanged();
}

void TerminalWidgetTerminal::bufferChanged() {
//     qDebug() << __FUNCTION__;
    emit terminalBufferChanged();
}

void TerminalWidgetTerminal::cursorPositionChanged() {
//     qDebug() << __FUNCTION__;
    emit terminalCursorPositionChanged();
}

void TerminalWidgetTerminal::titleChanged() {
    qDebug() << __FUNCTION__ << QString::fromUtf16(reinterpret_cast<const ushort*>(title().data()));
    emit terminalTitleChanged(QString::fromUtf16(reinterpret_cast<const ushort*>(title().data())));
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
TerminalWidget::TerminalWidget(const QString& shell, QWidget* parent)
: t(NULL)
, m_shell(shell)
, QWidget(parent) {
    setFont(QFont("Courier New"));
    startTerminal();
    setContextMenuPolicy(Qt::CustomContextMenu);
    setFocusPolicy(Qt::StrongFocus);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));
}
TerminalWidget::TerminalWidget(QWidget* parent)
: t(NULL)
, m_shell((getDefaultCmdInterpreter() + "\\cmd.exe").c_str())
, QWidget(parent) {
    QFont font("Lucida Console");
    QFontMetrics fm(font);
    qDebug() << __FUNCTION__ << fm.width("WWWWW WWWWW") << fm.width("||||| |||||") << fm.averageCharWidth();
    qDebug() << font.letterSpacing();
//     font.setBold(true);
//     font.setWeight(QFont::DemiBold);
//    font.setPointSizeF(font.pointSizeF() + .1);
//     font.setLetterSpacing(QFont::AbsoluteSpacing, .4);
    font.setPixelSize(14);
//     font.setStretch(QFont::SemiExpanded);
//     qDebug() << font.letterSpacing();
    qDebug() << __FUNCTION__ << fm.width("W") << fm.averageCharWidth();
    qDebug() << fm.height() << "X" << fm.width("W");
    setFont(font);
    startTerminal();
    setContextMenuPolicy(Qt::CustomContextMenu);
    setFocusPolicy(Qt::StrongFocus);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));
}

void TerminalWidget::startTerminal() {
    if(t != NULL) {
        disconnect(t, SIGNAL(terminalSizeChanged()), this, SLOT(resizeTerminal()));
        disconnect(t, SIGNAL(terminalBufferChanged()), this, SLOT(repaint()));
        disconnect(t, SIGNAL(terminalCursorPositionChanged()), this, SLOT(repaint()));
        disconnect(qApp, SIGNAL(aboutToQuit()), t, SLOT(endTerminal()));
        disconnect(t, SIGNAL(terminalQuit()), this, SLOT(startTerminal()));
        disconnect(t, SIGNAL(terminalTitleChanged(QString)), this, SIGNAL(titleChanged(QString)));
        delete t;
    }
    t = new TerminalWidgetTerminal;
    connect(t, SIGNAL(terminalSizeChanged()), this, SLOT(resizeTerminal()));
    connect(t, SIGNAL(terminalBufferChanged()), this, SLOT(repaint()));
    connect(t, SIGNAL(terminalCursorPositionChanged()), this, SLOT(repaint()));
    connect(qApp, SIGNAL(aboutToQuit()), t, SLOT(endTerminal()));
    connect(t, SIGNAL(terminalQuit()), this, SLOT(startTerminal()));
    connect(t, SIGNAL(terminalTitleChanged(QString)), this, SIGNAL(titleChanged(QString)));

    t->setCmd(m_shell.toLatin1().data());
    t->start();
    COORD c;
    c.X = width() / fontMetrics().width("W");
    c.Y = height() / fontMetrics().height();
    qDebug() << __FUNCTION__ << width() << "X" << height() << "=" << c.X << "X" << c.Y;
    t->setTerminalSize(c);
}

void TerminalWidget::resizeTerminal() {
//     qDebug() << "resizing terminal";
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

void TerminalWidget::resizeEvent(QResizeEvent* event) {
    COORD c;
//     qDebug() << __FUNCTION__;
    const QSize s = event->size();
    if(!s.isValid() || s.isNull()) return;

    c.X = s.width() / fontMetrics().width("W");
    c.Y = s.height() / fontMetrics().height();
    t->setTerminalSize(c);
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

QString TerminalWidget::title() const {
    return QString::fromUtf16(reinterpret_cast<const ushort*>(t->title().data()));
}

void TerminalWidget::setTitle(const QString& title) {
    t->setTitle(std::wstring((WCHAR*)title.utf16()));
}

void TerminalWidget::showContextMenu(const QPoint& pos)
{
    QMenu myMenu;
    QAction copyAction("Copy", this);
    QAction pasteAction("Paste", this);
    myMenu.addAction(&copyAction);
    myMenu.addAction(&pasteAction);

    const QClipboard* cb = QApplication::clipboard();
    const QMimeData* md = cb->mimeData();

    copyAction.setEnabled(false);
    pasteAction.setEnabled(md->hasText());

    QAction* selectedItem = myMenu.exec(mapToGlobal(pos));
    if(selectedItem == &pasteAction)
    {
        qDebug() << "sending clipboard content:" << md->text();
        t->sendText(reinterpret_cast<const WCHAR*>(md->text().utf16()));
    }
    else
    {
        // nothing was chosen
    }
}
