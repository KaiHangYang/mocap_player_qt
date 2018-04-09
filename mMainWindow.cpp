#include "mMainWindow.h"
#include "ui_window.h"
#include <QDebug>

mMainWindow::mMainWindow(QWidget *parent, int wnd_width, int wnd_height, QString title) : QMainWindow(parent), ui(new Ui::mMainWindow) {
    this->wnd_width = wnd_width;
    this->wnd_height = wnd_height;
    this->ui->setupUi(this);
    this->setWindowTitle(title);
    this->resize(this->wnd_width, this->wnd_height);

    this->gl_widget = new mGLWidget(this, this->size().width(), this->size().height());
    this->gl_widget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    this->setCentralWidget(this->gl_widget);
    this->gl_widget->adjustSize();
}

mMainWindow::~mMainWindow() {
    delete ui;
}
