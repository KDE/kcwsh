#include "qterminalwidget.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char**argv) {
    QApplication app(argc, argv);
    KcwSH::QtFrontend::TerminalWidget term;
    term.show();
    qDebug() << "shown!";
    return app.exec();
}