#include "mMainWindow.h"
#include "ui_window.h"
#include <QDebug>
#include <QGLFormat>
#include <QFileDialog>
#include <QAbstractItemView>
#include <QFileInfo>
#include <QFile>

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>


mMainWindow::mMainWindow(QWidget *parent, int wnd_width, int wnd_height, QString title) : QMainWindow(parent), ui(new Ui::mMainWindow) {
    this->wnd_width = wnd_width;
    this->wnd_height = wnd_height;
    this->file_dialog_extension = "BVH Files(*.bvh)";
    this->file_dialog_initial_dir = ".";
    this->camera_data_file_header = std::vector<QString>({"#M_CAMERA_DATA", "#M_CAMERA_FOLLOW_DATA"});

    this->ui->setupUi(this);
    this->setWindowTitle(title);
//    this->resize(this->wnd_width, this->wnd_height);

    this->cur_camera_type = 0;
    this->cur_camera_num[0] = 0;this->cur_camera_num[1] = 0;
    this->cur_camera_name_num[0] = 0;this->cur_camera_name_num[1] = 0;
    this->cur_capture_frame_sum = 0;

    // Set the grid widget to contain the widgets
    this->grid_widget = new QWidget(this);
    this->grid_layout = new QGridLayout();
    this->grid_layout->setContentsMargins(QMargins(0, 0, 0, 0));
    this->grid_layout->setSpacing(0);
    this->grid_widget->setLayout(this->grid_layout);

    this->buildGLView();
    this->buildProgressBar();
    this->buildToolBoxs();

    this->grid_layout->addWidget(this->video_box, 4, 0, 1, 1);
    this->grid_layout->addWidget(this->progress_bar, 4, 1, 1, 8);
    this->grid_layout->addWidget(this->tool_box_tabs, 0, 9, 5, 3);
    this->grid_layout->addWidget(this->gl_widget, 0, 0, 4, 9);
//    this->tool_box_tabs->hide();

    this->setCentralWidget(this->grid_widget);
    this->grid_widget->adjustSize();

    this->bindEvents();
    // fix the main window
    this->adjustSize();
    this->setFixedSize(this->size());
}
mMainWindow::~mMainWindow() {
    delete ui;
}
void mMainWindow::buildGLView() {
    // Set the OpenGL widget
    QGLFormat gl_format;
    gl_format.setVersion(3, 3);
    gl_format.setProfile(QGLFormat::CoreProfile); // use opengl core330
    gl_format.setSamples(4); // use anti-alias
    this->gl_widget = new mGLWidget(this->grid_widget, gl_format, this->wnd_width, this->wnd_height);
    this->gl_widget->setFixedWidth(this->wnd_width);
    this->gl_widget->setFixedHeight(this->wnd_height);
    this->gl_widget->adjustSize();
}
void mMainWindow::buildProgressBar() {
    // Set the progress bar widget
    this->progress_bar = new mProgressBarWidget(this->grid_widget);
    this->progress_bar->setFixedHeight(this->progress_bar->size().height());
//    this->progress_bar->setFixedWidth(wnd_width);
    //      Box for video control

    this->icon_play = QIcon(QPixmap(":/images/icon-play"));
    this->icon_pause = QIcon(QPixmap(":/images/icon-pause"));
    this->icon_redo = QIcon(QPixmap(":/images/icon-redo"));

    this->video_box = new QWidget(this->grid_widget);
    this->video_box_layout = new QGridLayout;
    this->video_box->setLayout(this->video_box_layout);
    this->tool_video_toggle_btn = new QPushButton(this->video_box);
    this->tool_video_toggle_btn->setIcon(this->icon_play);
    this->tool_video_reset_btn = new QPushButton(this->video_box);
    this->tool_video_reset_btn->setIcon(this->icon_redo);

    this->video_box_layout->addWidget(this->tool_video_toggle_btn, 0, 0, 1, 1);
    this->video_box_layout->addWidget(this->tool_video_reset_btn, 0, 1, 1, 1);
}
void mMainWindow::buildToolBoxs() {
    // Set the control box layout
    this->tool_box_tabs = new QTabWidget(this);
    this->buildToolBoxTab1();
    this->tool_box_tabs->addTab(this->tool_box, "Files Control");
    this->buildToolBoxTab2();
    this->tool_box_tabs->addTab(this->tool_box_2, "Cameras Control");
}
void mMainWindow::buildToolBoxTab1() {
    this->tool_box_layout = new QGridLayout;
    this->tool_box = new QWidget(this->tool_box_tabs);
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

    this->tool_box_layout->addWidget(this->file_box, 0, 0, 1, 1);

    // Disable some button
    this->tool_file_removeall_btn->setDisabled(true);
}
void mMainWindow::buildToolBoxTab2() {
    // Add tool_box 2
    this->tool_box_2_layout = new QGridLayout;
    this->tool_box_2 = new QWidget(this->tool_box_tabs);
    this->tool_box_2->setLayout(this->tool_box_2_layout);

    //      Box for camera control
    this->camera_box = new QGroupBox("Camera control:", this->tool_box_2);
    this->camera_box_layout = new QGridLayout;
    this->camera_box->setLayout(this->camera_box_layout);

    this->tool_camera_listview = new QListView(this->camera_box); // to show the camera pos name
    this->camera_list_model = new QStringListModel(this->tool_camera_listview);
    this->camera_list_follow_model = new QStringListModel(this->tool_camera_listview);

    if (this->cur_camera_type == 0) {
        this->tool_camera_listview->setModel(this->camera_list_model);
    }
    else if (this->cur_camera_type == 1) {
        this->tool_camera_listview->setModel(this->camera_list_follow_model);
    }

    this->tool_camera_listview->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->tool_camera_activate_btn = new QPushButton("Activate Camera", this->camera_box);
//    this->tool_camera_add_btn = new QPushButton("Add", this->camera_box);
    this->tool_camera_addcurr_btn = new QPushButton("Add Current", this->camera_box);
    this->tool_camera_follow_btn = new QPushButton("Follow", this->camera_box);
    this->tool_camera_focuson_btn = new QPushButton("Focus", this->camera_box);
    this->tool_camera_remove_btn = new QPushButton("Remove", this->camera_box);
    this->tool_camera_removeall_btn = new QPushButton("Remove All", this->camera_box);
    this->tool_camera_type_label = new QLabel("Camera Type: ", this->camera_box);
    this->tool_camera_type_combo = new QComboBox(this->camera_box);

    this->tool_camera_type_combo->addItem("Global");
    this->tool_camera_type_combo->addItem("Follow");
    if (this->cur_camera_type == 1) {
        this->tool_camera_type_combo->setCurrentIndex(1);
    }

    this->tool_camera_loadfromfile_btn = new QPushButton("Load", this->camera_box);
    this->tool_camera_savetofile_btn = new QPushButton("Save", this->camera_box);
    //    this->tool_camera_dialog = new QDialog(this);
    //    this->tool_camera_dialog->setWindowTitle("Add new camera");
    //    this->camera_dialog_layout = new QGridLayout;
    //    this->tool_camera_dialog->setLayout(this->camera_dialog_layout);
    //    this->tool_camera_dialog_name_lbl = new QLabel("Name:", this->tool_camera_dialog);
    //    this->tool_camera_dialog_name_ipt = new QLineEdit(this->tool_camera_dialog);
    //    this->tool_camera_dialog_name_ipt->setClearButtonEnabled(true);
    //    this->tool_camera_dialog_mat_lbl = new QLabel("Matrix:", this->tool_camera_dialog);
    //    this->tool_camera_dialog_mat_ipt = new QLineEdit(this->tool_camera_dialog);
    //    this->tool_camera_dialog_mat_ipt->setClearButtonEnabled(true);
    //    this->tool_camera_dialog_mat_show = new QTextEdit(this->tool_camera_dialog);
    //    this->tool_camera_dialog_add_btn = new QPushButton("Add", this->tool_camera_dialog);
    //    this->tool_camera_dialog_mat_show->setReadOnly(true);

    //    this->camera_dialog_layout->addWidget(this->tool_camera_dialog_mat_show, 0, 0, 3, 3);
    //    this->camera_dialog_layout->addWidget(this->tool_camera_dialog_name_lbl, 3, 0, 1, 1);
    //    this->camera_dialog_layout->addWidget(this->tool_camera_dialog_name_ipt, 3, 1, 1, 2);
    //    this->camera_dialog_layout->addWidget(this->tool_camera_dialog_mat_lbl, 4, 0, 1, 1);
    //    this->camera_dialog_layout->addWidget(this->tool_camera_dialog_mat_ipt, 4, 1, 1, 2);
    //    this->camera_dialog_layout->addWidget(this->tool_camera_dialog_add_btn, 5, 2, 1, 1);
    //    this->tool_camera_dialog->hide(); // the hide must be called after the dialog is created
    //    this->camera_box_layout->addWidget(this->tool_camera_add_btn, 0, 0, 1, 1);
    this->camera_box_layout->addWidget(this->tool_camera_addcurr_btn, 0, 0, 1, 1);
    this->camera_box_layout->addWidget(this->tool_camera_activate_btn, 0, 1, 1, 1);
    this->camera_box_layout->addWidget(this->tool_camera_remove_btn, 0, 2, 1, 1);
    this->camera_box_layout->addWidget(this->tool_camera_follow_btn, 1, 0, 1, 1);
    this->camera_box_layout->addWidget(this->tool_camera_focuson_btn, 1, 1, 1, 1);
//    this->camera_box_layout->addWidget(this->tool_camera_removeall_btn, 1, 1, 1, 1);
    this->camera_box_layout->addWidget(this->tool_camera_type_label, 2, 0, 1, 1);
    this->camera_box_layout->addWidget(this->tool_camera_type_combo, 2, 1, 1, 2);
    this->camera_box_layout->addWidget(this->tool_camera_listview, 3, 0, 4, 3);
    this->camera_box_layout->addWidget(this->tool_camera_loadfromfile_btn, 7, 1, 1, 1);
    this->camera_box_layout->addWidget(this->tool_camera_savetofile_btn, 7, 2, 1, 1);

    //      Box for capture control
    this->capture_box = new QGroupBox("Capture Control:", this->tool_box_2);
    this->capture_box_layout = new QGridLayout;
    this->capture_box->setLayout(this->capture_box_layout);
    this->tool_capture_dir_label = new QLabel("Save Dir:", this->capture_box);
    this->tool_capture_dir_input = new mLineEditWidget(this->capture_box);
    this->tool_capture_dir_input->setReadOnly(true);

    this->tool_capture_step_label = new QLabel("Step:", this->capture_box);
    this->tool_capture_step_input = new QLineEdit(this->capture_box);
    this->tool_capture_step_input->setValidator(new QIntValidator(0, 60, this->tool_capture_step_input));
    this->tool_capture_step_input->setPlaceholderText("0~60");
    this->tool_capture_capture_one = new QPushButton("Capture One", this->capture_box);
    this->tool_capture_capture_interval = new QPushButton("Capture All", this->capture_box);
    this->tool_capture_img_extension_label = new QLabel("Img Format: ", this->capture_box);
    this->tool_capture_img_extension_combox = new QComboBox(this->capture_box);
    this->tool_capture_img_extension_combox->addItem("jpg");
    this->tool_capture_img_extension_combox->addItem("png");

    this->capture_box_layout->addWidget(this->tool_capture_dir_label, 0, 0, 1, 1);
    this->capture_box_layout->addWidget(this->tool_capture_dir_input, 0, 1, 1, 3);
    this->capture_box_layout->addWidget(this->tool_capture_img_extension_label, 1, 0, 1, 1);
    this->capture_box_layout->addWidget(this->tool_capture_img_extension_combox, 1, 1, 1, 1);
    this->capture_box_layout->addWidget(this->tool_capture_step_label, 1, 2, 1, 1);
    this->capture_box_layout->addWidget(this->tool_capture_step_input, 1, 3, 1, 1);
    this->capture_box_layout->addWidget(this->tool_capture_capture_one, 2, 0, 1, 2);
    this->capture_box_layout->addWidget(this->tool_capture_capture_interval, 2, 2, 1, 2);

    this->tool_box_2_layout->addWidget(this->camera_box, 0, 0, 1, 1);
    this->tool_box_2_layout->addWidget(this->capture_box, 1, 0, 1, 1);

    this->tool_camera_removeall_btn->setDisabled(true);
    this->tool_camera_removeall_btn->hide();
}
void mMainWindow::bindEvents() {
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
    //    connect(this->tool_camera_add_btn, SIGNAL(clicked()), this, SLOT(cameraAddSlot()));
    connect(this->tool_camera_remove_btn, SIGNAL(clicked()), this, SLOT(cameraRemoveSlot()));
    connect(this->tool_camera_removeall_btn, SIGNAL(clicked()), this, SLOT(cameraRemoveAllSlot()));
    connect(this->tool_camera_activate_btn, SIGNAL(clicked()), this, SLOT(cameraActivateSlot()));
    connect(this->tool_camera_follow_btn, SIGNAL(clicked()), this, SLOT(cameraFollowSlot()));
    connect(this->tool_camera_focuson_btn, SIGNAL(clicked()), this, SLOT(cameraFocusSlot()));
    connect(this->camera_list_model, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(cameraEditNameSlot(QModelIndex,QModelIndex,QVector<int>)));
    connect(this->camera_list_follow_model, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(cameraEditNameSlot(QModelIndex,QModelIndex,QVector<int>)));
    connect(this->tool_camera_loadfromfile_btn, SIGNAL(clicked()), this, SLOT(cameraLoadFromFileSlot()));
    connect(this->tool_camera_savetofile_btn, SIGNAL(clicked()), this, SLOT(cameraSaveToFileSlot()));

    connect(this->tool_capture_dir_input, SIGNAL(lineEditOpenDirSignal()), this, SLOT(captureDirSlot()));
    connect(this->tool_capture_capture_one, SIGNAL(clicked()), this, SLOT(captureOneFrame()));
    connect(this->tool_camera_type_combo, SIGNAL(currentIndexChanged(int)), this, SLOT(cameraTypeChangeSlot(int)));
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
    // Change the button text
    this->tool_video_toggle_btn->setIcon(this->icon_play);
    emit signalOpenFile(file_path);
}

void mMainWindow::videoToggleSlot() {
    if (this->gl_widget->getPoseState() == 0) {
        this->tool_video_toggle_btn->setIcon(this->icon_pause);
        this->gl_widget->togglePose();
    }
    else if (this->gl_widget->getPoseState() == 1) {
        this->tool_video_toggle_btn->setIcon(this->icon_play);
        this->gl_widget->togglePose();
    }
}
void mMainWindow::videoResetSlot() {
    if (this->gl_widget->getIsHasPose()) {
        this->tool_video_toggle_btn->setIcon(this->icon_play);
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
    if (this->cur_camera_type == 0) {
        this->camera_list_model->insertRow(cur_row);
        QModelIndex index = this->camera_list_model->index(cur_row);

        this->camera_list_model->setData(index, "camera " + QString::number(this->cur_camera_name_num[this->cur_camera_type]++));

        this->camera_mat_arr.insert(this->camera_mat_arr.begin(), std::pair<QString, glm::mat4>(index.data().toString(), this->gl_widget->getCurExMat()));

        this->tool_camera_listview->setCurrentIndex(index);
        this->cur_camera_num[this->cur_camera_type] ++;
    }
    else if (this->cur_camera_type == 1) {
        if (this->gl_widget->getIsHasPose()) {
            int cur_row = 0;
            this->camera_list_follow_model->insertRow(cur_row);
            QModelIndex index = this->camera_list_follow_model->index(cur_row);

            this->camera_list_follow_model->setData(index, "camera " + QString::number(this->cur_camera_name_num[this->cur_camera_type]++));

            this->camera_vec_arr.insert(this->camera_vec_arr.begin(), std::pair<QString, glm::vec3>(index.data().toString(), this->gl_widget->getCurFollowVec()));

            this->tool_camera_listview->setCurrentIndex(index);
            this->cur_camera_num[this->cur_camera_type]++;
        }
        else {
            QMessageBox::critical(this, "Camera Error", "\"Follow\"(Camera Type) needs a pose in the scene!");
        }
    }
}
void mMainWindow::cameraAddSlot() {
    this->tool_camera_dialog->show();
}
void mMainWindow::cameraRemoveSlot() {
    QModelIndexList index_list = this->tool_camera_listview->selectionModel()->selectedIndexes();

    while (!index_list.isEmpty()) {
        int cur_row = index_list[0].row();
        if (this->cur_camera_type == 0) {
            this->camera_mat_arr.erase(this->camera_mat_arr.begin() + cur_row, this->camera_mat_arr.begin() + cur_row + 1);
            this->camera_list_model->removeRow(cur_row);
        }
        else if (this->cur_camera_type == 1) {
            this->camera_vec_arr.erase(this->camera_vec_arr.begin() + cur_row, this->camera_vec_arr.begin() + cur_row + 1);
            this->camera_list_follow_model->removeRow(cur_row);
        }
        index_list = this->tool_camera_listview->selectionModel()->selectedIndexes();
    }
}
void mMainWindow::cameraRemoveAllSlot() {
    if (this->cur_camera_type == 0) {
        this->camera_mat_arr.clear();
        this->camera_list_model->removeRows(0, this->camera_list_model->rowCount());
    }
    else if (this->cur_camera_type == 1) {
        this->camera_vec_arr.clear();
        this->camera_list_follow_model->removeRows(0, this->camera_list_follow_model->rowCount());
    }
}
void mMainWindow::cameraActivateSlot() {
    QModelIndexList index_list = this->tool_camera_listview->selectionModel()->selectedIndexes();
    if (!index_list.isEmpty()) {
        // first disable the camera follow
        int cur_row = index_list[0].row();
        if (this->cur_camera_type == 0) {
            this->cameraSetAllFollow(false);

            this->gl_widget->setCurExMat(this->camera_mat_arr[cur_row].second);
        }
        else if (this->cur_camera_type == 1) {
//            qDebug() << this->camera_vec_arr[cur_row].first << " " << index_list[0].data().toString();
            this->gl_widget->setCurFollowVec(this->camera_vec_arr[cur_row].second);
        }
    }
}
void mMainWindow::cameraFollowSlot() {
    if (this->gl_widget->getIsHasPose()) {
        bool is_follow = this->tool_camera_follow_btn->text() == "Follow";
        if (is_follow) {
            this->tool_camera_follow_btn->setText("Unfollow");
        }
        else {
            this->tool_camera_follow_btn->setText("Follow");
        }
        this->gl_widget->setFollowPerson(is_follow);
    }
    else {
        QMessageBox::critical(this, "Camera Error", "Need a pose in the scene!");
    }
}
void mMainWindow::cameraFocusSlot() {
    if (this->gl_widget->getIsHasPose()) {
        bool is_focus = this->tool_camera_focuson_btn->text() == "Focus";
        if (is_focus) {
            this->tool_camera_focuson_btn->setText("Unfocus");
        }
        else {
            this->tool_camera_focuson_btn->setText("Focus");
        }
        this->gl_widget->setFocusOnPerson(is_focus);
    }
    else {
        QMessageBox::critical(this, "Camera Error", "Need a pose in the scene!");
    }
}

void mMainWindow::cameraSetAllFollow(bool is_follow) {
    if (!is_follow) {
        this->tool_camera_follow_btn->setText("Follow");
        this->tool_camera_focuson_btn->setText("Focus");
    }
    else {
        this->tool_camera_follow_btn->setText("Unfollow");
        this->tool_camera_focuson_btn->setText("Unfocus");
    }
    this->gl_widget->setFollowPerson(is_follow);
    this->gl_widget->setFocusOnPerson(is_follow);

}
void mMainWindow::cameraEditNameSlot(QModelIndex cur_index, QModelIndex bottomright, QVector<int> roles) {
    if (this->cur_camera_type == 0) {
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
    else if (this->cur_camera_type == 1) {
        if (this->camera_list_follow_model->rowCount() != this->camera_vec_arr.size()) {
            return;
        }
        else {
            int cur_row = cur_index.row();
            if (cur_index.data().toString() != this->camera_vec_arr[cur_row].first) {
                this->camera_vec_arr[cur_row].first = cur_index.data().toString();
            }
        }
    }
}
void mMainWindow::cameraTypeChangeSlot(int index) {
    this->cur_camera_type = index;

    if (this->cur_camera_type == 0) {
        this->tool_camera_listview->setModel(this->camera_list_model);
    }
    else if (this->cur_camera_type == 1) {
        if (this->gl_widget->getIsHasPose()) {
            this->cameraSetAllFollow(true);
            this->tool_camera_listview->setModel(this->camera_list_follow_model);
        }
        else {
            QMessageBox::critical(this, "Camera Error", "Need a pose in the scene");
            this->tool_camera_type_combo->setCurrentIndex(0);
        }
    }
}

void mMainWindow::cameraLoadFromFileSlot() {
    QString read_file_name = QFileDialog::getOpenFileName(this, "Load Cameras Data", this->file_dialog_initial_dir, "*");
    if (!read_file_name.isEmpty()) {
        QFile cam_mat_file(read_file_name);
        if (cam_mat_file.open(QIODevice::ReadOnly)) {
            QTextStream file_stream(&cam_mat_file);
            QString file_string;
            glm::mat4 data_mat;
            glm::vec3 data_vec;
            float * data;
            file_stream >> file_string;
            if (file_string == this->camera_data_file_header[this->cur_camera_type]) {
                while (!file_stream.atEnd()) {
                    if (this->cur_camera_type == 0) {
                        data = &data_mat[0][0];
                        file_stream >> data[0] >> data[1] >> data[2] >> data[3] >>
                                       data[4] >> data[5] >> data[6] >> data[7] >>
                                       data[8] >> data[9] >> data[10] >> data[11] >>
                                       data[12] >> data[13] >> data[14] >> data[15];
                        file_string = file_stream.readLine();
                        file_string.remove(0, 1);
                        // Insert into the model list

                        this->camera_list_model->insertRow(0);
                        QModelIndex index = this->camera_list_model->index(0);
                        this->camera_list_model->setData(index, file_string);

                        this->camera_mat_arr.insert(this->camera_mat_arr.begin(), std::pair<QString, glm::mat4>(file_string, data_mat));
                        this->tool_camera_listview->setCurrentIndex(index);
                        this->cur_camera_num[this->cur_camera_type] ++;
                    }
                    else if (this->cur_camera_type == 1) {
                        data = &data_vec[0];
                        file_stream >> data[0] >> data[1] >> data[2];
                        file_string = file_stream.readLine();
                        file_string.remove(0, 1);
                        // Insert into the model list
                        this->camera_list_follow_model->insertRow(0);
                        QModelIndex index = this->camera_list_follow_model->index(0);
                        this->camera_list_follow_model->setData(index, file_string);

                        this->camera_vec_arr.insert(this->camera_vec_arr.begin(), std::pair<QString, glm::vec3>(file_string, data_vec));
                        this->tool_camera_listview->setCurrentIndex(index);
                        this->cur_camera_num[this->cur_camera_type] ++;
                    }
                }

            }
            else {
                qDebug() << "Not a valid camera data file";
            }
        }
    }
}
void mMainWindow::cameraSaveToFileSlot() {
    QString save_file_name = QFileDialog::getSaveFileName(this, "Save Cameras Data", this->file_dialog_initial_dir, "*");
    if (!save_file_name.isEmpty()) {
        QFile cam_mat_file(save_file_name);
        if (cam_mat_file.open(QIODevice::WriteOnly)) {
            QTextStream file_stream(&cam_mat_file);
            // write the file sign
            file_stream << this->camera_data_file_header[this->cur_camera_type] << "\n";

            if (this->cur_camera_type == 0) {
                for (int i = 0; i < this->camera_mat_arr.size(); ++i) {
                    glm::mat4 cam_ex_mat = this->camera_mat_arr[i].second;
                    file_stream << cam_ex_mat[0][0] << " " << cam_ex_mat[0][1] << " " << cam_ex_mat[0][2] << " " << cam_ex_mat[0][3] << " " << \
                                   cam_ex_mat[1][0] << " " << cam_ex_mat[1][1] << " " << cam_ex_mat[1][2] << " " << cam_ex_mat[1][3] << " " << \
                                   cam_ex_mat[2][0] << " " << cam_ex_mat[2][1] << " " << cam_ex_mat[2][2] << " " << cam_ex_mat[2][3] << " " << \
                                   cam_ex_mat[3][0] << " " << cam_ex_mat[3][1] << " " << cam_ex_mat[3][2] << " " << cam_ex_mat[3][3] << " " << this->camera_mat_arr[i].first << "\n";
                }
            }
            else if (this->cur_camera_type == 1) {
                for (int i = 0; i < this->camera_vec_arr.size(); ++i) {
                    glm::vec3 cam_ex_vec = this->camera_vec_arr[i].second;
                    file_stream << cam_ex_vec[0] << " " << cam_ex_vec[1] << " " << cam_ex_vec[2] << " " << this->camera_vec_arr[i].first << "\n";
                }
            }
            cam_mat_file.close();
        }
    }
}

void mMainWindow::captureDirSlot() {
    QString dir_name = QFileDialog::getExistingDirectory(this, "Select a directory to save the captured images.", ".");
    if (!dir_name.isEmpty()) {
        this->tool_capture_dir_input->setText(dir_name);
    }
}

void mMainWindow::captureOneFrame() {
    QString dir_name = this->tool_capture_dir_input->text();
    QFileInfo dir_info(dir_name);
    if (!dir_name.isEmpty() && dir_info.isDir()) {
        cv::Mat frame;
        this->gl_widget->captureFrame(frame);
        QString img_name = dir_name + "/" + QString::number(this->cur_capture_frame_sum) + "." + this->tool_capture_img_extension_combox->currentText();

        cv::imwrite(img_name.toStdString(), frame);
        this->cur_capture_frame_sum += 1;
    }
    else {
        QMessageBox::critical(this, "Path Error", "Save directory is not valid!");
    }
}
