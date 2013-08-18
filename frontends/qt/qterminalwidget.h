#ifndef qterminalwidget
#define qterminalwidget

#include "kcwsh_export.h"

#include <QWidget>

class TerminalWidgetTerminal;

namespace KcwSH {
namespace QtFrontend {

class KCWSHQT_EXPORT TerminalWidget : public QWidget {
        Q_OBJECT
    public:
        TerminalWidget(QWidget* parent = 0);
        QSize minimumSizeHint() const;
        QSize minimumSize() const;
        QSize sizeHint() const;
    private Q_SLOTS:
        void resizeTerminal();
        void startTerminal();
        void showContextMenu(const QPoint& p);
//         void aboutToQuit();
    private:
        void keyReleaseEvent(QKeyEvent* event);
        void keyPressEvent(QKeyEvent* event);
        void paintEvent(QPaintEvent* event);
        TerminalWidgetTerminal* t;
};
};
};
#endif /* qterminalwidget */
