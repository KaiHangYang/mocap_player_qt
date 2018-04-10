#ifndef MMAINWINDOW_H
#define MMAINWINDOW_H

#include <QMainWindow>
#include "mGLWidget.h"
#include <QGridLayout>
#include <QWidget>
#include <QProgressBar>
#include <QGroupBox>

namespace Ui {
class mMainWindow;
}

class mMainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit mMainWindow(QWidget *parent = 0, int wnd_width=960, int wnd_height=720, QString title="OpenGL Test");
    ~mMainWindow();
public slots:
    void openFile();
signals:
    void signalOpenFile(QString & file_path);
private:
    Ui::mMainWindow *ui;
    int wnd_width;
    int wnd_height;

    QGroupBox * tool_box;
    QProgressBar * progress_bar;
    QWidget * grid_widget;
    QGridLayout * grid_layout;
    // widgets
    mGLWidget * gl_widget;
};

#endif // MMAINWINDOW_H
