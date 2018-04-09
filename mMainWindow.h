#ifndef MMAINWINDOW_H
#define MMAINWINDOW_H

#include <QMainWindow>
#include "mGLWidget.h"

namespace Ui {
class mMainWindow;
}

class mMainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit mMainWindow(QWidget *parent = 0, int wnd_width=960, int wnd_height=720, QString title="OpenGL Test");
    ~mMainWindow();

private:
    Ui::mMainWindow *ui;
    int wnd_width;
    int wnd_height;

    mGLWidget * gl_widget;
};

#endif // MMAINWINDOW_H
