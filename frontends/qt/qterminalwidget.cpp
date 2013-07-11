#include "qterminalwidget.h"

#include <QKeyEvent>

using namespace KcwSH::Qt;

TerminalWidget::TerminalWidget(QWidget* parent)
: QWidget(parent)
, Terminal() {
}

void TerminalWidget::keyPressEvent(QKeyEvent* event) {
}