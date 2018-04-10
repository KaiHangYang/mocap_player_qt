#include "mMainWindow.h"
#include "ui_window.h"
#include <QDebug>
#include <QGLFormat>
#include <QFileDialog>
#include <QPushButton>
#include <QFrame>

mMainWindow::mMainWindow(QWidget *parent, int wnd_width, int wnd_height, QString title) : QMainWindow(parent), ui(new Ui::mMainWindow) {
    this->wnd_width = wnd_width;
    this->wnd_height = wnd_height;
    this->ui->setupUi(this);
    this->setWindowTitle(title);
    this->resize(this->wnd_width, this->wnd_height);
    // Set the grid widget to contain the widgets
    this->grid_widget = new QWidget(this);
    this->grid_layout = new QGridLayout();
    this->grid_layout->setContentsMargins(QMargins(0, 0, 0, 0));
    int cur_win_height;
    int cur_win_width;
    // Set the OpenGL widget
    QGLFormat gl_format;
    gl_format.setVersion(3, 3);
    gl_format.setProfile(QGLFormat::CoreProfile); // use opengl core330
    gl_format.setSamples(4); // use anti-alias
    this->gl_widget = new mGLWidget(this->grid_widget, gl_format, wnd_width, wnd_height);
    this->gl_widget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    this->gl_widget->setFixedWidth(wnd_width);
    this->gl_widget->setFixedHeight(wnd_height);
    this->grid_layout->addWidget(this->gl_widget, 0, 0);
    cur_win_height = this->gl_widget->size().height();
    cur_win_width = this->gl_widget->size().width();

    // Set the progress bar widget
    this->progress_bar = new QProgressBar(this->grid_widget);
    this->progress_bar->setFixedHeight(this->progress_bar->size().height());
    this->progress_bar->setFixedWidth(wnd_width);
    this->grid_layout->addWidget(this->progress_bar, wnd_height, 0);
    cur_win_height += this->progress_bar->size().height();

    // Set the control box layout
    this->tool_box = new QGroupBox();
    this->tool_box->setFixedWidth(200);
    this->tool_box->setLayout(new QVBoxLayout);
    this->grid_layout->addWidget(this->tool_box, 0, cur_win_width);

    QPushButton * tmp_button1 = new QPushButton("test1", this->tool_box);
    this->tool_box->layout()->addWidget(tmp_button1);
    QPushButton * tmp_button2 = new QPushButton("test2", this->tool_box);
    this->tool_box->layout()->addWidget(tmp_button2);
    QPushButton * tmp_button3 = new QPushButton("test2", this->tool_box);
    this->tool_box->layout()->addWidget(tmp_button3);


    cur_win_width += this->tool_box->size().width();

    this->grid_widget->setLayout(this->grid_layout);
    this->setCentralWidget(this->grid_widget);
    this->grid_widget->adjustSize();

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
