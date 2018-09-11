#ifndef MGLWIDGET_H
#define MGLWIDGET_H

#include "mSceneUtils.h"
#include "mMoCapReader.h"

#include <QGLWidget>
#include <QtOpenGL>
#include <QTimer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions_3_3_Core>
#include <vector>
#include <mPoseAdjuster.h>

class mGLWidget : public QGLWidget {
    Q_OBJECT
public:
    explicit mGLWidget(QWidget * parent=0, QGLFormat gl_format=QGLFormat(), int wnd_width=960, int wnd_height=720);
    ~mGLWidget();

    int getClickJointIndexOnCurPose(glm::vec2 click_pos);

    int getPoseState();
    bool getIsHasPose();

    int getCurCameraType();
    void setCurCameraType(int camera_type);

    const mCamera * getCurCamera();
    void setCurCamera(const mCamera * camera);

    void setCurInMat(glm::vec4 proj_vec);
    glm::mat4 getCurExMat();
    void setCurExMat(glm::mat4 cur_ex_mat);
    glm::vec3 getCurFollowVec();
    void setCurFollowVec(glm::vec3 cur_follow_vec);
    void setFollowPerson(bool is_follow);
    void setFocusOnPerson(bool is_focus);
    bool getFollowPerson();
    bool getFocusOnPerson();
    void getSplittedCameras(int camera_num, std::vector<glm::vec3> &splitted_cameras);

    void setUseFloor(bool is_with_floor);

    void captureFrame(std::vector<const mCamera *> cameras);
    void captureAllFrames(std::vector<const mCamera *> cameras);

    void resetCapture();
    void stopCaptureAll();
    void setPoseChangeStep(float change_step);
    void setJitter(float jitter_size);
    void setAngleJitter(float jitter_size);
    void setUseShading(bool use_shading);
    void setVerticalAngle(float angle);
    void setVisualizeCameras(std::vector<const mCamera *> cameras_arr);
    void setIsChangingPose(bool is_changing, std::vector<glm::vec3> cur_pose=std::vector<glm::vec3>());
    void resetChangingPose();
    std::vector<glm::vec3> getCurPoseJoints();
    void setCurPoseJoints(const std::vector<glm::vec3> & cur_pose);
    void setIsShowingJitters(bool is_showing_jitters);
    void setWithRenderedImg(bool is_with);
public slots:
    void changePoseFile(QString & file_name, int cur_dataset_num);
    void togglePose();
    void startPose();
    void stopPose();
    void tempPausePose();
    void tempStartPose();

    void resetPose();
    void setPose(float ratio);
signals:
    void doubleClickPoseToggleSignal();
    void progressDisplaySignal(int cur_num, int total, bool is_reset);
    void saveCapturedFrameSignal(cv::Mat & frame, int cur_frame, int cur_num, QString suffix);
    void saveCapturedLabelSignal(std::vector<glm::vec2> labels_2d, std::vector<glm::vec3> labels_3d, int cur_frame, int cur_num, bool is_raw);
    void changePoseFileSignal();
protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    void mouseDoubleClickEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
private:
    void draw();
    /**************** Progress control **************/
    void sendProgress(bool is_reset);
    /************************************************/

    bool is_with_floor;
    bool use_shading;
    bool is_changing_pose;
    bool is_showing_jitters;

    int cur_selected_joint_index;

    std::vector<double> cur_pose_angles;
    std::vector<double> _cur_pose_angles;
    std::vector<double> cur_pose_bonelengths;
    glm::vec3 cur_pose_joint_root;
    glm::vec2 cur_click_pos;

    int wnd_width;
    int wnd_height;

    mSceneUtils * scene;
    mMoCapData * mocap_data;
    mMoCapReader mocap_reader;
    QOpenGLVertexArrayObject * VAO;
    QOpenGLFunctions_3_3_Core * core_func;
    QTimer * timer_for_update;

    /**************** Pose control ******************/
    int pose_state; // -1 no pose; 0 pause; 1 start; 2 reset; (after reset the state is 1)
    int temp_pose_state; // for the prograss bar control
    bool is_has_pose;
    std::vector<glm::vec3> cur_pose_joints;
    std::vector<glm::vec3> cur_pose_joints_raw;
    float pose_change_step;
    float pose_jitter_range;
    float pose_angle_jitter_range;
    mPoseAdjuster * pose_adjuster;
    bool is_with_rendered_img;
    /************************************************/

    /**************** Camera visualization **************/
    std::vector<const mCamera *> cur_visualization_cameras;
    /****************************************************/

    /************** Capture Control *****************/
    std::vector<const mCamera *> cur_capture_cameras;
    bool is_set_capture_frame;
    bool is_capture_all;
    /************************************************/
};

#endif // MGLWIDGET_H
