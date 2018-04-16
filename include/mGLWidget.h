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

class mGLWidget : public QGLWidget {
    Q_OBJECT
public:
    explicit mGLWidget(QWidget * parent=0, QGLFormat gl_format=QGLFormat(), int wnd_width=960, int wnd_height=720);
    ~mGLWidget();
    int getPoseState();
    bool getIsHasPose();
    glm::mat4 getCurExMat();
    void setCurExMat(glm::mat4 cur_ex_mat);
    glm::vec3 getCurFollowVec();
    void setCurFollowVec(glm::vec3 cur_follow_vec);
    void setFollowPerson(bool is_follow);
    void setFocusOnPerson(bool is_focus);
    void getSplittedCameras(int camera_num, std::vector<glm::vec3> &splitted_cameras);
    void setFollowDefault();
    void setUseFloor(bool is_with_floor);
    void captureFrame(const std::vector<glm::vec3> & view_vecs);
    void captureFrame(const std::vector<glm::mat4> & view_mats);
    void captureAllFrames(const std::vector<glm::vec3> & view_vecs);
    void captureAllFrames(const std::vector<glm::mat4> & view_mats);
    void resetCapture();
    void stopCapture();
    void setPoseChangeStep(float change_step);

public slots:
    void changePoseFile(QString & file_name, int cur_dataset_num);
    void togglePose();
    void startPose();
    void stopPose();
    void resetPose();
    void tempStartPose();
    void tempPausePose();
    void setPose(float ratio);
signals:
    void doubleClickPoseToggleSignal();
    void progressDisplaySignal(int cur_num, int total, bool is_reset);
    void saveCapturedFrameSignal(cv::Mat & frame, std::vector<glm::vec2> labels_2d, std::vector<glm::vec3> labels_3d, int cur_frame, int cur_num);
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

    glm::mat4 cam_in_mat;
    glm::mat4 cam_ex_mat;
    bool is_ar;
    bool is_with_floor;

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
    int temp_pose_state;
    bool is_has_pose;
    std::vector<glm::vec3> cur_pose_joints;
    float pose_change_step;
    /************************************************/

    /**************** Progress control **************/
    void sendProgress(bool is_reset);
    /************************************************/

    /************** Capture Control *****************/
    int cur_capture_sum;
    std::vector<glm::mat4> cur_capture_view_mats;
    std::vector<glm::vec3> cur_capture_view_vecs;
    bool is_set_capture_frame;
    bool is_set_capture_all_frames;
    int cur_capture_type;
    /************************************************/
};

#endif // MGLWIDGET_H
