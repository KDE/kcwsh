/* Copyright 2013-2014  Patrick Spendrin <ps_ml@gmx.de>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
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