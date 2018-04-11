#include "mProgressBarWidget.h"
#include <QDebug>

mProgressBarWidget::mProgressBarWidget(QWidget * parent): QProgressBar(parent) {
    this->is_pause = false;
}

mProgressBarWidget::~mProgressBarWidget() {

}

void mProgressBarWidget::mousePressEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {

        this->is_pause = true;
        int total_size = this->size().width();
        int cur_pos = event->pos().x();
        cur_pos = (cur_pos < 0)?0:((cur_pos > total_size)?total_size:cur_pos);
        emit setTemporaryStateSignal(true);
        emit setProgressSignal((float) cur_pos / total_size);
    }
}

void mProgressBarWidget::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        int total_size = this->size().width();
        int cur_pos = event->pos().x();
        cur_pos = (cur_pos < 0)?0:((cur_pos > total_size)?total_size:cur_pos);

        emit setProgressSignal((float) cur_pos / total_size);
    }
}

void mProgressBarWidget::mouseReleaseEvent(QMouseEvent *event) {

    if (this->is_pause) {
        this->is_pause = false;
        emit setTemporaryStateSignal(false);
    }
}

void mProgressBarWidget::setMaxNum(int total_frame) {
    this->setRange(0, total_frame);
    this->setFormat("%v / " + QString::number(total_frame));
}

void mProgressBarWidget::setCurNum(int cur_num) {
    this->setValue(cur_num);
}
