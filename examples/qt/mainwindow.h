#ifndef mainwindow_h
#define mainwindow_h

#include "qterminalwidget.h"
#include <QWidget>
#include <QObject>
#include <QList>

class QTabWidget;
class QComboBox;
class QLineEdit;

class MainWindow : public QWidget {
    Q_OBJECT
    public:
        MainWindow();
    public Q_SLOTS:
        void addNewTab();
        void tabTitleChanged(const QString& t);
        void updateTitleEdit(int i);
        void updateTerminalTitle(const QString& s);
    private:
        QList<KcwSH::QtFrontend::TerminalWidget*> m_terminals;
        QTabWidget* m_tabWidget;
        QComboBox* m_shellSelector;
        QLineEdit* m_titleEdit;
};
#endif /* mainwindow_h */