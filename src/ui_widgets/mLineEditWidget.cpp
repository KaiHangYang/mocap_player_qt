#include "mLineEditWidget.h"
#include <QDebug>

mLineEditWidget::mLineEditWidget(QWidget *parent) : QLineEdit(parent) {}

void mLineEditWidget::mouseDoubleClickEvent(QMouseEvent * event) {
    emit lineEditOpenDirSignal();
}
