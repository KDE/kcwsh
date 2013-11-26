#ifndef qterminalwidget
#define qterminalwidget

#include "kcwsh_export.h"

#include <QWidget>
#include <QString>

class TerminalWidgetTerminal;

namespace KcwSH {
namespace QtFrontend {

class KCWSHQT_EXPORT TerminalWidget : public QWidget {
        Q_OBJECT
    public:
        TerminalWidget(const QString& shell, QWidget* parent = 0);
        TerminalWidget(QWidget* parent = 0);
        QSize minimumSizeHint() const;
        QSize minimumSize() const;
        QSize sizeHint() const;

        QString title() const;
        void setTitle(const QString& t);

    Q_SIGNALS:
        void titleChanged(QString t);

    private Q_SLOTS:
        void resizeTerminal();
        void startTerminal();
        void showContextMenu(const QPoint& p);
//         void aboutToQuit();
    private:
        void keyReleaseEvent(QKeyEvent* event);
        void keyPressEvent(QKeyEvent* event);
        void paintEvent(QPaintEvent* event);
        void resizeEvent(QResizeEvent* event);
        QString m_shell;
        TerminalWidgetTerminal* t;
};
};
};
#endif /* qterminalwidget */
