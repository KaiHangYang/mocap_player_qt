#include "mMainWindow.h"
#include "ui_window.h"
#include <QDebug>
#include <QGLFormat>
#include <QFileDialog>
#include <QAbstractItemView>
#include <QFileInfo>


mMainWindow::mMainWindow(QWidget *parent, int wnd_width, int wnd_height, QString title) : QMainWindow(parent), ui(new Ui::mMainWindow) {
    this->wnd_width = wnd_width;
    this->wnd_height = wnd_height;
    this->file_dialog_extension = "BVH Files(*.bvh)";
    this->file_dialog_initial_dir = ".";
    this->ui->setupUi(this);
    this->setWindowTitle(title);
    this->resize(this->wnd_width, this->wnd_height);

    this->cur_camera_num = 0;
    this->cur_camera_name_num = 0;

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
    this->progress_bar = new mProgressBarWidget(this->grid_widget);
    this->progress_bar->setFixedHeight(this->progress_bar->size().height());
    this->progress_bar->setFixedWidth(wnd_width);

    // Set the control box layout
    this->tool_box_layout = new QGridLayout;
    this->tool_box = new QGroupBox();
    this->tool_box->setLayout(this->tool_box_layout);
    //      Box for file
    this->file_box = new QGroupBox("Files Control:", this->tool_box);
    this->file_box_layout = new QGridLayout;
    this->file_box->setLayout(this->file_box_layout);
    this->tool_file_add_btn = new QPushButton("Add", this->file_box);
    this->tool_file_remove_btn = new QPushButton("Remove", this->file_box);
    this->tool_file_removeall_btn = new QPushButton("Remove All", this->file_box);
    this->tool_file_listview = new QListView(this->file_box);
    this->file_list_model = new QStringListModel(this->tool_file_listview);
    this->tool_file_listview->setModel(this->file_list_model);
    this->tool_file_listview->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->tool_file_listview->setSelectionMode(QAbstractItemView::ExtendedSelection);

    this->file_box_layout->addWidget(this->tool_file_add_btn, 0, 0, 1, 1);
    this->file_box_layout->addWidget(this->tool_file_remove_btn, 0, 1, 1, 1);
    this->file_box_layout->addWidget(this->tool_file_removeall_btn, 0, 2, 1, 1);
    this->file_box_layout->addWidget(this->tool_file_listview, 1, 0, 4, 3);
    //      Box for video control
    this->video_box = new QGroupBox("Poses control:", this->tool_box);
    this->video_box_layout = new QGridLayout;
    this->video_box->setLayout(this->video_box_layout);
    this->tool_video_toggle_btn = new QPushButton("Start", this->video_box);
    this->tool_video_reset_btn = new QPushButton("Reset", this->video_box);
    this->video_box_layout->addWidget(this->tool_video_toggle_btn, 0, 0, 1, 1);
    this->video_box_layout->addWidget(this->tool_video_reset_btn, 0, 1, 1, 1);

    //      Box for camera control
    this->camera_box = new QGroupBox("Camera control:", this->tool_box);
    this->camera_box_layout = new QGridLayout;
    this->camera_box->setLayout(this->camera_box_layout);

    this->tool_camera_listview = new QListView(this->camera_box); // to show the camera pos name
    this->camera_list_model = new QStringListModel(this->tool_camera_listview);
    this->tool_camera_listview->setModel(this->camera_list_model);
    this->tool_camera_listview->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->tool_camera_activate_btn = new QPushButton("Activate Camera", this->camera_box);
    this->tool_camera_add_btn = new QPushButton("Add", this->camera_box);
    this->tool_camera_addcurr_btn = new QPushButton("Add Current", this->camera_box);
    this->tool_camera_remove_btn = new QPushButton("Remove", this->camera_box);
    this->tool_camera_removeall_btn = new QPushButton("Remove All", this->camera_box);
    this->tool_camera_loadfromfile_btn = new QPushButton("Load", this->camera_box);
    this->tool_camera_savetofile_btn = new QPushButton("Save", this->camera_box);
    this->tool_camera_dialog = new QDialog(this);
    this->tool_camera_dialog->setWindowTitle("Add new camera");
    this->camera_dialog_layout = new QGridLayout;
    this->tool_camera_dialog->setLayout(this->camera_dialog_layout);
    this->tool_camera_dialog_name_lbl = new QLabel("Name:", this->tool_camera_dialog);
    this->tool_camera_dialog_name_ipt = new QLineEdit(this->tool_camera_dialog);
    this->tool_camera_dialog_name_ipt->setClearButtonEnabled(true);
    this->tool_camera_dialog_mat_lbl = new QLabel("Matrix:", this->tool_camera_dialog);
    this->tool_camera_dialog_mat_ipt = new QLineEdit(this->tool_camera_dialog);
    this->tool_camera_dialog_mat_ipt->setClearButtonEnabled(true);
    this->tool_camera_dialog_mat_show = new QTextEdit(this->tool_camera_dialog);
    this->tool_camera_dialog_add_btn = new QPushButton("Add", this->tool_camera_dialog);
    this->tool_camera_dialog_mat_show->setReadOnly(true);

    this->camera_dialog_layout->addWidget(this->tool_camera_dialog_mat_show, 0, 0, 3, 3);
    this->camera_dialog_layout->addWidget(this->tool_camera_dialog_name_lbl, 3, 0, 1, 1);
    this->camera_dialog_layout->addWidget(this->tool_camera_dialog_name_ipt, 3, 1, 1, 2);
    this->camera_dialog_layout->addWidget(this->tool_camera_dialog_mat_lbl, 4, 0, 1, 1);
    this->camera_dialog_layout->addWidget(this->tool_camera_dialog_mat_ipt, 4, 1, 1, 2);
    this->camera_dialog_layout->addWidget(this->tool_camera_dialog_add_btn, 5, 2, 1, 1);
    this->tool_camera_dialog->hide(); // the hide must be called after the dialog is created


    this->camera_box_layout->addWidget(this->tool_camera_add_btn, 0, 0, 1, 1);
    this->camera_box_layout->addWidget(this->tool_camera_remove_btn, 0, 1, 1, 1);
    this->camera_box_layout->addWidget(this->tool_camera_removeall_btn, 0, 2, 1, 1);
    this->camera_box_layout->addWidget(this->tool_camera_addcurr_btn, 1, 0, 1, 1);
    this->camera_box_layout->addWidget(this->tool_camera_activate_btn, 1, 1, 1, 2);
    this->camera_box_layout->addWidget(this->tool_camera_listview, 2, 0, 4, 3);
    this->camera_box_layout->addWidget(this->tool_camera_loadfromfile_btn, 6, 1, 1, 1);
    this->camera_box_layout->addWidget(this->tool_camera_savetofile_btn, 6, 2, 1, 1);

    this->tool_box_layout->addWidget(this->file_box, 0, 0, 1, 1);
    this->tool_box_layout->addWidget(this->video_box, 1, 0, 1, 1);
    this->tool_box_layout->addWidget(this->camera_box, 2, 0, 1, 1);

    this->grid_layout->addWidget(this->gl_widget, 0, 0, 4, 6);
    this->grid_layout->addWidget(this->progress_bar, 4, 0, 1, 6);
    this->grid_layout->addWidget(this->tool_box, 0, 6, 3, 2);

    this->grid_widget->setLayout(this->grid_layout);
    this->setCentralWidget(this->grid_widget);
    this->grid_widget->adjustSize();

    // Disable some button
    this->tool_file_removeall_btn->setDisabled(true);
    this->tool_camera_removeall_btn->setDisabled(true);

    // Set the events
    connect(this->ui->openAct, SIGNAL(triggered()), this, SLOT(fileAddSlot()));
    connect(this, SIGNAL(signalOpenFile(QString&)), this->gl_widget, SLOT(changePoseFile(QString&)));
    connect(this->ui->exitAct, SIGNAL(triggered()), this, SLOT(close()));

    // Tool box event
    connect(this->tool_file_add_btn, SIGNAL(clicked()), this, SLOT(fileAddSlot()));
    connect(this->tool_file_remove_btn, SIGNAL(clicked()), this, SLOT(fileRemoveSlot()));
    connect(this->tool_file_removeall_btn, SIGNAL(clicked()), this, SLOT(fileRemoveAllSlot()));
    connect(this->tool_file_listview, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(fileActivatedSlot(QModelIndex)));
    // Pose box event
    connect(this->tool_video_toggle_btn, SIGNAL(clicked()), this, SLOT(videoToggleSlot()));
    connect(this->tool_video_reset_btn, SIGNAL(clicked()), this, SLOT(videoResetSlot()));
    connect(this->gl_widget, SIGNAL(doubleClickPoseToggleSignal()), this, SLOT(videoToggleSlot()));
    // progress bar event
    connect(this->gl_widget, SIGNAL(progressDisplaySignal(int,int,bool)), this, SLOT(progressBarDisplaySlot(int,int,bool)));
    connect(this->progress_bar, SIGNAL(setProgressSignal(float)), this, SLOT(progressBarSetSlot(float)));
    connect(this->progress_bar, SIGNAL(setTemporaryStateSignal(bool)), this, SLOT(poseTemporaryStateSlot(bool)));
    // camera box
    connect(this->tool_camera_addcurr_btn, SIGNAL(clicked()), this, SLOT(cameraAddCurrSlot()));
    connect(this->tool_camera_add_btn, SIGNAL(clicked()), this, SLOT(cameraAddSlot()));
    connect(this->tool_camera_remove_btn, SIGNAL(clicked()), this, SLOT(cameraRemoveSlot()));
    connect(this->tool_camera_removeall_btn, SIGNAL(clicked()), this, SLOT(cameraRemoveAllSlot()));
    connect(this->tool_camera_activate_btn, SIGNAL(clicked()), this, SLOT(cameraActivateSlot()));
    connect(this->camera_list_model, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(cameraEditNameSlot(QModelIndex,QModelIndex,QVector<int>)));

    // fix the main window
    this->adjustSize();
    this->setFixedSize(this->size());
}

mMainWindow::~mMainWindow() {
    delete ui;
}
/********************** Implementation of Slots **********************/

void mMainWindow::fileAddSlot() {
    QStringList file_names = QFileDialog::getOpenFileNames(this, "Add Files", this->file_dialog_initial_dir, this->file_dialog_extension);
    if (file_names.size() > 0) {

        // Store the last directories
        QFileInfo cur_dir_info(file_names.at(0));
        this->file_dialog_initial_dir = cur_dir_info.absoluteDir().absolutePath();

        for (int i = 0; i < file_names.size(); ++i) {
            QStringList cur_list = this->file_list_model->stringList();
            bool is_exist = false;
            // check if the file exists
            for (int j = 0; j < cur_list.size(); ++j) {
                if (cur_list.at(j) == file_names.at(i)) {
                    is_exist = true;
                    break;
                }
            }
            if (is_exist) {
                continue;
            }
            int cur_row = 0;
            this->file_list_model->insertRow(cur_row);
            QModelIndex index = this->file_list_model->index(cur_row);

            this->file_list_model->setData(index, file_names.at(i));
            this->tool_file_listview->setCurrentIndex(index);
        }
    }
}
void mMainWindow::fileRemoveSlot() {
    QModelIndexList index_list = this->tool_file_listview->selectionModel()->selectedIndexes();

    while (!index_list.isEmpty()) {
        this->file_list_model->removeRow(index_list[0].row());
        index_list = this->tool_file_listview->selectionModel()->selectedIndexes();
    }
}
void mMainWindow::fileRemoveAllSlot() {
    this->file_list_model->removeRows(0, this->file_list_model->rowCount());
}
void mMainWindow::fileActivatedSlot(QModelIndex index) {
    QString file_path = index.data().toString();
    this->tool_video_toggle_btn->setText("Start");
    emit signalOpenFile(file_path);
}

void mMainWindow::videoToggleSlot() {
    if (this->gl_widget->getPoseState() == 0) {
        this->tool_video_toggle_btn->setText("Pause");
        this->gl_widget->togglePose();
    }
    else if (this->gl_widget->getPoseState() == 1) {
        this->tool_video_toggle_btn->setText("Start");
        this->gl_widget->togglePose();
    }
}
void mMainWindow::videoResetSlot() {
    if (this->gl_widget->getIsHasPose()) {
        this->tool_video_toggle_btn->setText("Start");
        this->gl_widget->resetPose();
    }
}
void mMainWindow::progressBarDisplaySlot(int cur_num, int total, bool is_reset) {
    if (is_reset) {
        this->progress_bar->setMaxNum(total);
    }
    this->progress_bar->setCurNum(cur_num);
}

void mMainWindow::progressBarSetSlot(float cur_ratio) {
    // here set the framepose
    this->gl_widget->setPose(cur_ratio);
}

void mMainWindow::poseTemporaryStateSlot(bool is_pause) {
    if (is_pause) {
        this->gl_widget->tempPausePose();
    }
    else {
        this->gl_widget->tempStartPose();
    }
}
// Slot for camera control
void mMainWindow::cameraAddCurrSlot() {
    int cur_row = 0;
    this->camera_list_model->insertRow(cur_row);
    QModelIndex index = this->camera_list_model->index(cur_row);

    this->camera_list_model->setData(index, "camera " + QString::number(this->cur_camera_name_num++));

    this->camera_mat_arr.insert(this->camera_mat_arr.begin(), std::pair<QString, glm::mat4>(index.data().toString(), this->gl_widget->getCurExMat()));

    this->tool_camera_listview->setCurrentIndex(index);
    this->cur_camera_num ++;
}
void mMainWindow::cameraAddSlot() {
    this->tool_camera_dialog->show();
}
void mMainWindow::cameraRemoveSlot() {
    QModelIndexList index_list = this->tool_camera_listview->selectionModel()->selectedIndexes();

    while (!index_list.isEmpty()) {
        int cur_row = index_list[0].row();
        this->camera_mat_arr.erase(this->camera_mat_arr.begin() + cur_row, this->camera_mat_arr.begin() + cur_row + 1);
        this->camera_list_model->removeRow(cur_row);
        index_list = this->tool_camera_listview->selectionModel()->selectedIndexes();
    }
}
void mMainWindow::cameraRemoveAllSlot() {
    this->camera_mat_arr.clear();
    this->camera_list_model->removeRows(0, this->camera_list_model->rowCount());
}
void mMainWindow::cameraActivateSlot() {
    QModelIndexList index_list = this->tool_camera_listview->selectionModel()->selectedIndexes();
    if (!index_list.isEmpty()) {
        int cur_row = index_list[0].row();
        qDebug() << index_list[0].data().toString() << " " << this->camera_mat_arr[cur_row].first;
        this->gl_widget->setCurExMat(this->camera_mat_arr[cur_row].second);
    }
}
void mMainWindow::cameraEditNameSlot(QModelIndex cur_index, QModelIndex bottomright, QVector<int> roles) {
    if (this->camera_list_model->rowCount() != this->camera_mat_arr.size()) {
        return;
    }
    else {
        int cur_row = cur_index.row();
        if (cur_index.data().toString() != this->camera_mat_arr[cur_row].first) {
            this->camera_mat_arr[cur_row].first = cur_index.data().toString();
        }
    }
}
void mMainWindow::cameraLoadFromFileSlot() {

}
void mMainWindow::cameraSaveToFileSlot() {

}
