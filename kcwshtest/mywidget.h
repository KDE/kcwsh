#ifndef _MYWIDGET_H_
#define _MYWIDGET_H_

#include <QtGui/QWidget>
#include <QtCore/QObject>
#include <QtCore/QString>

class QProcess;
class QPushButton;
class QLabel;
class QHBoxLayout;

class MyWidget : public QWidget {
    Q_OBJECT
    public:
        MyWidget();
    public slots:
        void start();
        void updateText();
    private:
        QString m_text;
        QProcess *m_proc;
        QPushButton *m_startButton;
        QLabel *m_label;
        QHBoxLayout *m_layout;
};

#endif