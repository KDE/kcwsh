#ifndef mainwindow_h
#define mainwindow_h

#include "qterminalwidget.h"
#include <QWidget>
#include <QObject>
#include <QList>

class QTabWidget;

class MainWindow : public QWidget {
    Q_OBJECT
    public:
        MainWindow();
    public Q_SLOTS:
        void addNewTab();
    private:
        QList<KcwSH::QtFrontend::TerminalWidget*> m_terminals;
        QTabWidget* m_tabWidget;
};
#endif /* mainwindow_h */