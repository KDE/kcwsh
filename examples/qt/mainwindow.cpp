#include "mainwindow.h"

#include <QApplication>
#include <QComboBox>
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QLineEdit>
#include <QPushButton>
#include <QTabWidget>

#include <windows.h>
#include <Shlobj.h>

#include "qterminalwidget.h"

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

MainWindow::MainWindow()
: m_tabWidget(new QTabWidget)
, QWidget() {
    QPushButton *button = new QPushButton(QIcon(qApp->applicationDirPath() + "/../share/qkcwsh/icons/list-add.png"), "");
    connect(button, SIGNAL(clicked()), this, SLOT(addNewTab()));
    QVBoxLayout *vlayout = new QVBoxLayout(this);
    QHBoxLayout *hlayout = new QHBoxLayout;

    m_shellSelector = new QComboBox;
    m_shellSelector->insertItem(0, "cmd", QString::fromStdString(getDefaultCmdInterpreter() + "\\cmd.exe"));
    m_shellSelector->insertItem(1, "powershell", QString::fromStdString(getDefaultCmdInterpreter() + "\\WindowsPowerShell\\v1.0\\powershell.exe"));
    m_shellSelector->insertItem(2, "msys", QString::fromStdString("C:\\kde\\km64\\msys\\bin\\sh.exe --login -i"));

    m_terminals.append(new KcwSH::QtFrontend::TerminalWidget);
    m_terminals.last()->setMinimumSize(560, 350);
    m_tabWidget->addTab(m_terminals.last(), m_terminals.last()->title());
    connect(m_terminals.last(), SIGNAL(titleChanged(QString)), this, SLOT(tabTitleChanged(QString)));

    m_titleEdit = new QLineEdit;
    m_titleEdit->setText(m_terminals.last()->title());

    hlayout->addWidget(m_titleEdit);
    hlayout->insertStretch(-1);
    hlayout->addWidget(m_shellSelector);
    hlayout->addWidget(button);

    vlayout->addLayout(hlayout);
    vlayout->addWidget(m_tabWidget);
    connect(m_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(updateTitleEdit(int)));
    connect(m_titleEdit, SIGNAL(textEdited(const QString&)), this, SLOT(updateTerminalTitle(const QString&)));
}

void MainWindow::updateTitleEdit(int i) {
//     qDebug() << __FUNCTION__ << i << "/" << m_terminals.size();
    if(m_terminals.size() <= i) return;

    m_titleEdit->setText(m_terminals[i]->title());
}

void MainWindow::updateTerminalTitle(const QString& s) {
    m_terminals[m_tabWidget->currentIndex()]->setTitle(m_titleEdit->text());
}

void MainWindow::tabTitleChanged(const QString& t) {
    KcwSH::QtFrontend::TerminalWidget* tw = qobject_cast<KcwSH::QtFrontend::TerminalWidget*>(sender());
    if(tw != 0) {
        bool found = false;
        for(int i = 0; i < m_terminals.size(); i++) {
            if(tw == m_terminals[i]) {
                m_tabWidget->setTabText(i, m_terminals[i]->title());
                m_titleEdit->setText(m_terminals[i]->title());
                m_tabWidget->setCurrentIndex(i);
                found = true;
                break;
            }
        };
        if(!found) qDebug() << "could not find correct tab!";
    } else {
        qDebug() << "could not get sender object in slot!";
    }
}

void MainWindow::addNewTab() {
    m_terminals.append(new KcwSH::QtFrontend::TerminalWidget(m_shellSelector->itemData(m_shellSelector->currentIndex()).toString()));
    m_terminals.last()->setMinimumSize(560, 350);
    m_tabWidget->addTab(m_terminals.last(), m_terminals.last()->title());
    connect(m_terminals.last(), SIGNAL(titleChanged(QString)), this, SLOT(tabTitleChanged(QString)));
}