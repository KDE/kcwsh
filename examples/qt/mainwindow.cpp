#include "mainwindow.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QTabWidget>

#include "qterminalwidget.h"

MainWindow::MainWindow()
: m_tabWidget(new QTabWidget)
, QWidget() {
    QPushButton *button = new QPushButton("Add New Console");
    connect(button, SIGNAL(clicked()), this, SLOT(addNewTab()));
    QVBoxLayout *layout = new QVBoxLayout(this);
    m_terminals.append(new KcwSH::QtFrontend::TerminalWidget);
    m_terminals.last()->setMinimumSize(560, 350);
    m_tabWidget->addTab(m_terminals.last(), QString("%1").arg(m_terminals.size()));
    layout->addWidget(button);
    layout->addWidget(m_tabWidget);
}

void MainWindow::addNewTab() {
    m_terminals.append(new KcwSH::QtFrontend::TerminalWidget);
    m_terminals.last()->setMinimumSize(560, 350);
    m_tabWidget->addTab(m_terminals.last(), QString("%1").arg(m_terminals.size()));
}