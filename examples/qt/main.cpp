#include "qterminalwidget.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char**argv) {
    QApplication app(argc, argv);
    KcwSH::QtFrontend::TerminalWidget term;
    term.setMinimumSize(560, 350);      // the standard terminal size of 80x25
    term.show();
    qDebug() << "shown!";
    return app.exec();
}