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
#include <QTabWidget>
#include <QIcon>
#include <QComboBox>
#include <QCheckBox>

#include "mProgressBarWidget.h"
#include "mLineEditWidget.h"
#include "mCamera.h"

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
    void fileHighlightCurrentSlot();
    void fileDataSetChangeSlot(int cur_dataset);
    void updateFileListNumShow();
    void fileListLoadFromFileSlot();
    void fileListSaveToFileSlot();

    void videoToggleSlot();
    void videoStartSlot();
    void videoStopSlot();
    void videoResetSlot();
    void progressBarDisplaySlot(int cur_num, int total, bool is_reset);
    void progressBarSetSlot(float cur_ratio);
    void poseTemporaryStateSlot(bool is_pause);
    void renderCameraTypeChange();
    void cameraAddSlot();
    void cameraAddCurrSlot();
    void cameraRemoveSlot();
    void cameraRemoveAllSlot();
    void cameraLoadFromFileSlot();
    void cameraSaveToFileSlot();
    void cameraActivateSlot();
    void cameraFollowSlot();
    void cameraFocusSlot();
    void cameraUpdataFollowNFocus();
    void cameraSetAllFollow(bool is_follow);
    void cameraEditNameSlot(QModelIndex topleft, QModelIndex bottomright, QVector<int> roles);
    void captureDirSlot();
    void captureOneFrame();
    void captureAllFrames();
    void captureCurrentAll();
    void captureStop();
    void cameraTypeChangeSlot(int index);
    void cameraSplitCircleSlot();
    void cameraSetVerticalAngle();
    void cameraSetSplitPrefix();
    void cameraVisualizeToggleSlot();
    void cameraVisualizeShowSlot();
    void cameraVisualizeHideSlot();
    void sceneFloorSlot();
    void saveFramesSlot(cv::Mat & frames, int cur_frame, int cur_num, QString suffix);
    void saveLabelsSlot(std::vector<glm::vec2> labels_2d, std::vector<glm::vec3> labels_3d, int cur_frame, int cur_num, bool is_raw);
    void changeNextPoseSlot();
    void poseSetChangeSize();
    void poseSetJitterSize();
    void poseSetAngleJitterSize();
    void renderSetUseShading();
    void poseChangerStartSlot();

    void poseChangerResetSlot();
    void poseChangerResetCounterSlot();
    void poseChangerAddSlot();
    void poseChangerRemoveSlot();
    void poseChangerUseSlot();
    void poseChangerSaveSlot();
    void poseChangerLoadSlot();
    void poseChangerEditNameSlot(QModelIndex topleft, QModelIndex bottomright, QVector<int> roles);

    void showJittersToggleSlot();
    void captureWithRenderedImgToggle(bool);
private:
    /************** Functions for build the GUI *************/
    void buildGLView();
    void buildProgressBar();
    void buildToolBoxs();
    void buildToolBoxTab1();
    void buildToolBoxTab2();
    void buildToolBoxTab3();
    void bindEvents();
    void setFileListControlState(bool is_disable);

    void _poseChangerStart(bool is_start);
    void _poseChangerAdd(QString pose_name, const std::vector<glm::vec3> & pose_joints);

    /********************************************************/

    /************** Function to simplify the implementation ***************/

    void fileListAddItem(QString file_path);
    /**********************************************************************/

    Ui::mMainWindow *ui;
    int wnd_width;
    int wnd_height;

    /****** Just for store *****/
    int cur_pose_file_index[2];
    int cur_camera_type; // 0 is the global type, 1 is the follow type
    int cur_camera_name_num[2];
    int cur_camera_num[2];
    std::vector<std::pair<QString, const mCamera *>> camera_mat_arr;
    std::vector<std::pair<QString, const mCamera *>> camera_vec_arr;

    QString file_dialog_extension;
    QString file_dialog_initial_dir;
    std::vector<QString> camera_data_file_header;
    std::vector<QStringListModel *> file_list_model;
    int cur_dataset_num;

    QStringListModel * camera_list_model;
    QStringListModel * camera_list_follow_model;

    QIcon icon_play;
    QIcon icon_pause;
    QIcon icon_redo;

    QString split_camera_prefix;
    int pose_changer_pose_num;

    std::vector<std::pair<QString, std::vector<glm::vec3>>> pose_changer_pose_arr;
    QString pose_data_file_header;

    bool is_setting_camera_focus_on_root;

    /******** Main widget ******/
    mProgressBarWidget * progress_bar;
    QWidget * grid_widget;
    QGridLayout * grid_layout;

    /***** Tool box widget *****/
    QTabWidget * tool_box_tabs;

    QWidget * tool_box;
    QWidget * tool_box_2;
    QWidget * tool_box_3;
    QGroupBox * file_box;
    QGroupBox * pose_box;
    QGroupBox * render_box;
    QWidget * video_box;
    QGroupBox * camera_box;
    QGroupBox * pose_changer_box;

    QGridLayout * tool_box_layout;
    QGridLayout * tool_box_2_layout;
    QGridLayout * tool_box_3_layout;

    QGridLayout * file_box_layout;
    QGridLayout * pose_box_layout;
    QGridLayout * render_box_layout;
    QGridLayout * video_box_layout;
    QGridLayout * camera_box_layout;
    QGridLayout * pose_changer_box_layout;

    QPushButton * tool_file_add_btn;
    QPushButton * tool_file_remove_btn;
    QPushButton * tool_file_removeall_btn;
    QPushButton * tool_file_highlight_current;
    QPushButton * tool_file_list_load_btn;
    QPushButton * tool_file_list_save_btn;
    QLabel * tool_file_list_num_label;

    QLabel * tool_file_dataset_label;
    QComboBox * tool_file_dataset_combo;

    QListView * tool_file_listview;
    QListView * tool_camera_listview;

    QLabel * tool_pose_change_step_label;
    QLineEdit * tool_pose_change_step_input;
    QPushButton * tool_pose_change_step_btn;

    QLabel * tool_render_type_label;
    QPushButton * tool_render_type_btn;
    QLabel * tool_render_floor_label;
    QPushButton * tool_render_floor_btn;
    QLabel * tool_render_camera_type_lbl;
    QPushButton * tool_render_camera_type_btn;
    QLabel * tool_render_show_jitters_lbl;
    QPushButton * tool_render_show_jitters_btn;

    QLabel * tool_pose_jitter_size_label;
    QLineEdit * tool_pose_jitter_size_input;
    QPushButton * tool_pose_jitter_size_btn;

    QLabel * tool_pose_angle_jitter_size_label;
    QLineEdit * tool_pose_angle_jitter_size_input;
    QPushButton * tool_pose_angle_jitter_size_btn;

    QPushButton * tool_video_toggle_btn;
    QPushButton * tool_video_reset_btn;

    QPushButton * tool_camera_loadfromfile_btn;
    QPushButton * tool_camera_savetofile_btn;
    QLabel * tool_camera_visualize_lbl;
    QPushButton * tool_camera_visualize_btn;
    QPushButton * tool_camera_add_btn;
    QPushButton * tool_camera_addcurr_btn;
    QPushButton * tool_camera_activate_btn;
    QPushButton * tool_camera_remove_btn;
    QPushButton * tool_camera_follow_btn;
    QPushButton * tool_camera_focuson_btn;

    QLabel * tool_camera_type_label;
    QComboBox * tool_camera_type_combo;

    QLabel * tool_camera_split_label;
    QPushButton * tool_camera_split_btn;
    QLineEdit * tool_camera_split_num;
    QLabel * tool_camera_split_prefix_label;
    QPushButton * tool_camera_split_prefix_btn;
    QLineEdit * tool_camera_split_prefix_input;


    QLineEdit * tool_camera_vertical_angle_input;
    QPushButton * tool_camera_vertical_angle_btn;
    QLabel * tool_camera_vertical_angle_label;

    // camera add dialog
    QDialog * tool_camera_dialog;
    QGridLayout * camera_dialog_layout;
    QLineEdit * tool_camera_dialog_name_ipt;
    QLineEdit * tool_camera_dialog_mat_ipt;
    QLabel * tool_camera_dialog_name_lbl;
    QLabel * tool_camera_dialog_mat_lbl;
    QTextEdit * tool_camera_dialog_mat_show;
    QPushButton * tool_camera_dialog_add_btn;

    // Pose Changer box
    QPushButton * tool_pose_changer_start_btn;
    QPushButton * tool_pose_changer_reset_pose_btn;
    QPushButton * tool_pose_changer_add_btn;
    QPushButton * tool_pose_changer_remove_btn;
    QPushButton * tool_pose_changer_save_btn;
    QPushButton * tool_pose_changer_load_btn;
    QPushButton * tool_pose_changer_use_btn;
    QPushButton * tool_pose_changer_reset_counter_btn;

    QListView * tool_pose_changer_listview;
    QStringListModel * tool_pose_changer_listview_model;

    // capture box
    QGroupBox * capture_box;
    QGridLayout * capture_box_layout;

    QLabel * tool_capture_dir_label;
    mLineEditWidget * tool_capture_dir_input;

    QPushButton * tool_capture_capture_one;
    QPushButton * tool_capture_capture_all;
    QPushButton * tool_capture_capture_currentall;
    QPushButton * tool_capture_stop;

    QLabel * tool_capture_img_extension_label;
    QComboBox * tool_capture_img_extension_combox;

    QCheckBox * tool_capture_with_rendered_img_cb;

    /**************************/

    // OpenGL widgets
    mGLWidget * gl_widget;
};

#endif // MMAINWINDOW_H
