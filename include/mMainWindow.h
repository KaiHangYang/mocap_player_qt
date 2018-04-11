#ifndef MMAINWINDOW_H
#define MMAINWINDOW_H

#include <QMainWindow>
#include "mGLWidget.h"
#include <QGridLayout>
#include <QWidget>
#include <QGroupBox>
#include <QPushButton>
#include <QListView>
#include <QStringListModel>
#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QTextEdit>
#include <vector>

#include "mProgressBarWidget.h"

namespace Ui {
class mMainWindow;
}

class mMainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit mMainWindow(QWidget *parent = 0, int wnd_width=960, int wnd_height=720, QString title="OpenGL Test");
    ~mMainWindow();
public slots:
    void fileAddSlot();
    void fileRemoveSlot();
    void fileRemoveAllSlot();
    void fileActivatedSlot(QModelIndex index);
    void videoToggleSlot();
    void videoResetSlot();
    void progressBarDisplaySlot(int cur_num, int total, bool is_reset);
    void progressBarSetSlot(float cur_ratio);
    void poseTemporaryStateSlot(bool is_pause);

    void cameraAddSlot();
    void cameraAddCurrSlot();
    void cameraRemoveSlot();
    void cameraRemoveAllSlot();
    void cameraLoadFromFileSlot();
    void cameraSaveToFileSlot();
    void cameraActivateSlot();
    void cameraEditNameSlot(QModelIndex topleft, QModelIndex bottomright, QVector<int> roles);
signals:
    void signalOpenFile(QString & file_path);
private:
    Ui::mMainWindow *ui;
    int wnd_width;
    int wnd_height;

    /****** Just for store *****/
    int cur_camera_name_num;
    int cur_camera_num;
    std::vector<std::pair<QString, glm::mat4>> camera_mat_arr;

    QString file_dialog_extension;
    QString file_dialog_initial_dir;
    QString camera_data_file_header;
    QStringListModel * file_list_model;
    QStringListModel * camera_list_model;
    /******** Main widget ******/
    mProgressBarWidget * progress_bar;
    QWidget * grid_widget;
    QGridLayout * grid_layout;

    /***** Tool box widget *****/
    QGroupBox * tool_box;
    QGroupBox * file_box;
    QGroupBox * video_box;
    QGroupBox * camera_box;

    QGridLayout * tool_box_layout;
    QGridLayout * file_box_layout;
    QGridLayout * video_box_layout;
    QGridLayout * camera_box_layout;

    QPushButton * tool_file_add_btn;
    QPushButton * tool_file_remove_btn;
    QPushButton * tool_file_removeall_btn;
    QListView * tool_file_listview;
    QListView * tool_camera_listview;

    QPushButton * tool_video_toggle_btn;
    QPushButton * tool_video_reset_btn;

    QPushButton * tool_camera_loadfromfile_btn;
    QPushButton * tool_camera_savetofile_btn;
    QPushButton * tool_camera_add_btn;
    QPushButton * tool_camera_addcurr_btn;
    QPushButton * tool_camera_activate_btn;
    QPushButton * tool_camera_remove_btn;
    QPushButton * tool_camera_removeall_btn;
    // camera add dialog
    QDialog * tool_camera_dialog;
    QGridLayout * camera_dialog_layout;
    QLineEdit * tool_camera_dialog_name_ipt;
    QLineEdit * tool_camera_dialog_mat_ipt;
    QLabel * tool_camera_dialog_name_lbl;
    QLabel * tool_camera_dialog_mat_lbl;
    QTextEdit * tool_camera_dialog_mat_show;
    QPushButton * tool_camera_dialog_add_btn;
    /**************************/

    // OpenGL widgets
    mGLWidget * gl_widget;
};

#endif // MMAINWINDOW_H
