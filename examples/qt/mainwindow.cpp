#include "mainwindow.h"

#include <QApplication>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIcon>
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

    m_terminals.append(new KcwSH::QtFrontend::TerminalWidget);
    m_terminals.last()->setMinimumSize(560, 350);
    m_tabWidget->addTab(m_terminals.last(), QString("%1").arg(m_terminals.size()));

    m_shellSelector = new QComboBox;
    m_shellSelector->insertItem(0, "cmd", QString::fromStdString(getDefaultCmdInterpreter() + "\\cmd.exe"));
    m_shellSelector->insertItem(1, "powershell", QString::fromStdString(getDefaultCmdInterpreter() + "\\WindowsPowerShell\\v1.0\\powershell.exe"));
    m_shellSelector->insertItem(2, "msys", QString::fromStdString("C:\\kde\\km64\\msys\\bin\\sh.exe --login -i"));

    hlayout->addWidget(m_shellSelector);
    hlayout->insertStretch(-1);
    hlayout->addWidget(button);
    vlayout->addLayout(hlayout);
    vlayout->addWidget(m_tabWidget);
}

void MainWindow::addNewTab() {
    m_terminals.append(new KcwSH::QtFrontend::TerminalWidget(m_shellSelector->itemData(m_shellSelector->currentIndex()).toString()));
    m_terminals.last()->setMinimumSize(560, 350);
    m_tabWidget->addTab(m_terminals.last(), QString("%1").arg(m_terminals.size()));
}