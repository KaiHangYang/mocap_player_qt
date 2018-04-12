#ifndef MLINEEDITWIDGET_H
#define MLINEEDITWIDGET_H

#include <QLineEdit>
#include <QWidget>
#include <QMouseEvent>

class mLineEditWidget: public QLineEdit {
    Q_OBJECT
public:
    explicit mLineEditWidget(QWidget * parent);
    ~mLineEditWidget() {}
signals:
    void lineEditOpenDirSignal();
protected:
    void mouseDoubleClickEvent(QMouseEvent *);
};


#endif // MLINEEDITWIDGET_H
