#ifndef qterminalwidget
#define qterminalwidget

#include "terminal.h"
#include "kcwsh_export.h"

#include <QWidget>

namespace KcwSH {
namespace Qt {
class KCWSHQT_EXPORT TerminalWidget : public QWidget {
    public:
        TerminalWidget(QWidget* parent = 0);
    private:
        void keyPressEvent(QKeyEvent* event);
        Terminal* m_term;
};
};
};
#endif /* qterminalwidget */
