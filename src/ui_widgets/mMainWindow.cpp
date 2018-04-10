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
    // Set the grid widget to contain the widgets
    this->grid_widget = new QWidget(this);
    this->grid_layout = new QGridLayout();
    this->grid_layout->setContentsMargins(QMargins(0, 0, 0, 0));
    this->grid_layout->setSpacing(0);
    // Set the OpenGL widget
    QGLFormat gl_format;
    gl_format.setVersion(3, 3);
    gl_format.setProfile(QGLFormat::CoreProfile); // use opengl core330
    gl_format.setSamples(4); // use anti-alias
    this->gl_widget = new mGLWidget(this->grid_widget, gl_format, wnd_width, wnd_height);
    this->gl_widget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    this->gl_widget->setFixedWidth(wnd_width);
    this->gl_widget->setFixedHeight(wnd_height);

    // Set the progress bar widget
    this->progress_bar = new QProgressBar(this->grid_widget);
    this->progress_bar->setFixedHeight(this->progress_bar->size().height());
    this->progress_bar->setFixedWidth(wnd_width);

    // Set the control box layout
    this->tool_box_layout = new QGridLayout;
    this->tool_box = new QGroupBox();
    this->tool_box->setLayout(this->tool_box_layout);
    //      Box for file
    this->file_box = new QGroupBox("File choose:", this->tool_box);
    this->file_box_layout = new QGridLayout;
    this->file_box->setLayout(this->file_box_layout);
    this->tool_file_add_btn = new QPushButton("Add", this->file_box);
    this->tool_file_remove_btn = new QPushButton("Remove", this->file_box);
    this->tool_file_removeall_btn = new QPushButton("Remove All", this->file_box);
    this->tool_file_listview = new QListView(this->file_box);

    this->file_box_layout->addWidget(this->tool_file_add_btn, 0, 0, 1, 1);
    this->file_box_layout->addWidget(this->tool_file_remove_btn, 0, 1, 1, 1);
    this->file_box_layout->addWidget(this->tool_file_removeall_btn, 0, 2, 1, 1);
    this->file_box_layout->addWidget(this->tool_file_listview, 1, 0, 4, 3);
    //      Box for video control
    this->video_box = new QGroupBox("Pose control:", this->tool_box);
    this->video_box_layout = new QGridLayout;
    this->video_box->setLayout(this->video_box_layout);
    this->tool_video_start_btn = new QPushButton("Start", this->video_box);
    this->tool_video_stop_btn = new QPushButton("Stop", this->video_box);
    this->tool_video_reset_btn = new QPushButton("Reset", this->video_box);
    this->video_box_layout->addWidget(this->tool_video_start_btn, 0, 0, 1, 1);
    this->video_box_layout->addWidget(this->tool_video_stop_btn, 0, 1, 1, 1);
    this->video_box_layout->addWidget(this->tool_video_reset_btn, 0, 2, 1, 1);

    this->tool_box_layout->addWidget(this->file_box, 0, 0, 1, 1);
    this->tool_box_layout->addWidget(this->video_box, 1, 0, 1, 1);

    this->grid_layout->addWidget(this->gl_widget, 0, 0, 4, 6);
    this->grid_layout->addWidget(this->progress_bar, 4, 0, 1, 6);
    this->grid_layout->addWidget(this->tool_box, 0, 6, 2, 2);

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
