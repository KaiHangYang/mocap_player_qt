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
    void setFollowPerson(bool is_follow);
    void captureFrame(cv::Mat & frame);
public slots:
    void changePoseFile(QString & file_name);
    void togglePose();
    void resetPose();
    void tempStartPose();
    void tempPausePose();
    void setPose(float ratio);
signals:
    void doubleClickPoseToggleSignal();
    void progressDisplaySignal(int cur_num, int total, bool is_reset);
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
    /************************************************/

    /**************** Progress control **************/
    void sendProgress(bool is_reset);
    /************************************************/
};

#endif // MGLWIDGET_H
