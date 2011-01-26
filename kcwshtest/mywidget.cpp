#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QHBoxLayout>
#include <QtCore/QProcess>
#include <QtCore/QStringList>

#include "mywidget.h"

MyWidget::MyWidget() : m_proc(0), m_startButton(0), m_label(0), m_layout(0), QWidget(0) {
    m_layout = new QHBoxLayout(this);
    m_startButton = new QPushButton("Start Process");
    m_label = new QLabel;
    m_label->setText("_");
    m_layout->addWidget(m_startButton);
    m_layout->addWidget(m_label);
    connect(m_startButton, SIGNAL(clicked()), this, SLOT(start()));
    this->resize(200, 100);
}

void MyWidget::start() {
    m_proc = new QProcess( this );
    connect(m_proc, SIGNAL(readyReadStandardOutput()), this, SLOT(updateText()));
    m_proc->start("kcwsh", QStringList()/* << "C:\\Windows\\SysWOW64\\cmd.exe" */);
    m_proc->waitForStarted();
//    m_proc->write("\\e[");
    m_proc->write("dir\r\n");
}

void MyWidget::updateText() {
    m_text += m_proc->readAllStandardOutput();
    m_label->setText(m_text);
}