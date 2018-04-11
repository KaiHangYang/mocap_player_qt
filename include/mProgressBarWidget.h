#ifndef MPROGRESSBARWIDGET_H
#define MPROGRESSBARWIDGET_H

#include <QWidget>
#include <QProgressBar>
#include <QMouseEvent>

class mProgressBarWidget : public QProgressBar
{
    Q_OBJECT
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
signals:
    void setProgressSignal(float cur_ratio);
    void setTemporaryStateSignal(bool is_pause);

public:
    explicit mProgressBarWidget(QWidget * parent);
    ~mProgressBarWidget();

    void setMaxNum(int total_num);
    void setCurNum(int cur_num);
private:
    bool is_pause;
};

#endif // MPROGRESSBARWIDGET_H
