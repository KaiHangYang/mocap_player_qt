#include "mMainWindow.h"
#include "ui_window.h"
#include <QDebug>
#include <QGLFormat>
#include <QFileDialog>

mMainWindow::mMainWindow(QWidget *parent, int wnd_width, int wnd_height, QString title) : QMainWindow(parent), ui(new Ui::mMainWindow) {
    this->wnd_width = wnd_width;
    this->wnd_height = wnd_height;
    this->ui->setupUi(this);
    this->setWindowTitle(title);
    this->resize(this->wnd_width, this->wnd_height);

    // This is useful on the MacOS
    QGLFormat gl_format;
    gl_format.setVersion(3, 3);
    gl_format.setProfile(QGLFormat::CoreProfile); // use opengl core330
    gl_format.setSamples(4); // use anti-alias
    // for initializing the glwidget

    this->gl_widget = new mGLWidget(gl_format, this, this->size().width(), this->size().height());
    this->gl_widget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    this->setCentralWidget(this->gl_widget);
    this->gl_widget->adjustSize();

    connect(this->ui->openAct, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(this, SIGNAL(signalOpenFile(QString&)), this->gl_widget, SLOT(changePoseFile(QString&)));
}

mMainWindow::~mMainWindow() {
    delete ui;
}


void mMainWindow::openFile() {
//    QString file_name = QFileDialog::getOpenFileName(this, "Open File", ".", "BVH Files(*.bvh);C3D Files(*.c3d)");
    QString file_name = QFileDialog::getOpenFileName(this, "Open File", ".", "*");
    if (!file_name.isEmpty()) {
        emit signalOpenFile(file_name);
    }
}
