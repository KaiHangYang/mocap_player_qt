#include "mGLWidget.h"
#include <QtWidgets>
#include <QDebug>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "mRotateUtils.h"

mGLWidget::mGLWidget(QWidget * parent, QGLFormat gl_format, int wnd_width, int wnd_height) : QGLWidget(gl_format, parent) {
    this->wnd_width = wnd_width;
    this->wnd_height = wnd_height;
    // set cam_in_mat cam_ex_mat and is_ar here

    this->pose_state = -1;
    this->temp_pose_state = 1;
    this->is_has_pose = false;
    this->cur_pose_joints = std::vector<glm::vec3>();

    this->is_ar = false;
    this->cam_in_mat = glm::transpose(glm::perspective(glm::radians(45.f), (float)this->wnd_width / this->wnd_height, 0.01f, 1000000.f));
    this->cam_ex_mat = glm::transpose(glm::lookAt(glm::vec3(0, 10.f, 300.f), glm::vec3(0, 10.f, 0), glm::vec3(0, 1, 0)));
    this->mocap_data = new mMoCapData;

    this->setFocusPolicy(Qt::StrongFocus);
    this->timer_for_update = new QTimer(this);
    connect(timer_for_update, SIGNAL(timeout()), this, SLOT(update()));
}
mGLWidget::~mGLWidget() {
    this->scene->~mSceneUtils();
}

void mGLWidget::initializeGL() {
    // Before this function, the opengl context has already been prepared.
    this->VAO = new QOpenGLVertexArrayObject(this);
    this->core_func = new QOpenGLFunctions_3_3_Core;

    this->core_func->initializeOpenGLFunctions();
    this->VAO->create();

    this->scene = new mSceneUtils(this->VAO, this->core_func, this->wnd_width, this->wnd_height, this->cam_in_mat, this->cam_ex_mat, this->is_ar);
    glViewport(0, 0, this->wnd_width, this->wnd_height);
    glClearColor(0.4627450980392157f, 0.5882352941176471f, 0.8980392156862745f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    this->timer_for_update->start(20);
}

void mGLWidget::resizeGL(int width, int height) {
    this->wnd_width = width;
    this->wnd_height = height;
    glViewport(0, 0, this->wnd_width, this->wnd_height);
}

void mGLWidget::mousePressEvent(QMouseEvent * event) {
    mCamRotate::mouse_button_callback(event);
}

void mGLWidget::mouseMoveEvent(QMouseEvent *event) {
    mCamRotate::mouse_move_callback(event);

}
void mGLWidget::mouseDoubleClickEvent(QMouseEvent *event) {
    emit doubleClickPoseToggleSignal();
}
void mGLWidget::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_W:
            // go up
            this->scene->moveCamera(2);
            break;
        case Qt::Key_S:
            // go down
            this->scene->moveCamera(-2);
            break;
        case Qt::Key_A:
            this->scene->moveCamera(1);
            break;
        case Qt::Key_D:
            this->scene->moveCamera(-1);
            break;
    }
}
void mGLWidget::wheelEvent(QWheelEvent *event) {
    if (event->angleDelta().y() > 0) {
        this->scene->moveCamera(-3);
    }
    else if (event->angleDelta().y() < 0) {
        this->scene->moveCamera(3);
    }
}
void mGLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.4627450980392157f, 0.5882352941176471f, 0.8980392156862745f, 1.0f);
    this->draw();
}

void mGLWidget::draw() {
    glm::mat4 cur_ex_r_mat, cur_ex_t_mat, cur_rotate_mat;
    this->scene->getCurExMat(cur_ex_r_mat, cur_ex_t_mat);
    cur_rotate_mat = mCamRotate::getRotateMat(this->wnd_width, this->wnd_height, cur_ex_r_mat);
    this->scene->rotateCamrea(cur_rotate_mat);

    if (this->pose_state == 1 && this->temp_pose_state == 1) {
        this->sendProgress(false);
        this->mocap_data->getOneFrame(cur_pose_joints, this->scene->getRawExMat());
    }

    this->scene->render(cur_pose_joints);
}

int mGLWidget::getPoseState() {
    return this->pose_state;
}
bool mGLWidget::getIsHasPose() {
    return this->is_has_pose;
}
void mGLWidget::sendProgress(bool is_reset) {
    if (this->is_has_pose) {
        emit progressDisplaySignal(this->mocap_data->getCurFrame(), this->mocap_data->getTotalFrame(), is_reset);
    }
}
glm::mat4 mGLWidget::getCurExMat() {
    return this->scene->getCurExMat();
}
void mGLWidget::setCurExMat(glm::mat4 cur_ex_mat) {
    this->scene->setCurExMat(cur_ex_mat);
}
void mGLWidget::setFollowPerson(bool is_follow) {
    this->scene->setFollowPerson(is_follow);
}
void mGLWidget::setFocusOnPerson(bool is_focus) {
    this->scene->setFocusOnCenter(is_focus);
}
void mGLWidget::captureFrame(cv::Mat & frame) {
    this->scene->captureFrame(frame);
}
/*************** Implementation of slots *****************/
void mGLWidget::changePoseFile(QString & file_name) {
    if (this->mocap_reader.parse(file_name, 0, this->mocap_data)) {
        // Get the first frame pose
        this->is_has_pose = true;
        this->pose_state = 0;
        this->sendProgress(true);
        this->mocap_data->getOneFrame(cur_pose_joints, this->scene->getRawExMat());
    }
    else {
        this->is_has_pose = false;
        this->pose_state = -1;
    }
}

void mGLWidget::togglePose() {
    if (this->is_has_pose) {
        if (this->pose_state == 1) {
            this->pose_state = 0;
        }
        else if (this->pose_state == 0){
            this->pose_state = 1;
        }
    }
}
void mGLWidget::resetPose() {
    if (this->is_has_pose) {
        this->pose_state = 0;
        this->mocap_data->resetCounter();
        this->sendProgress(false);
        // Reset to the first frame pose
        this->mocap_data->getOneFrame(this->cur_pose_joints, this->scene->getRawExMat());
    }
}

void mGLWidget::setPose(float ratio) {
    int cur_frame_num = this->mocap_data->getTotalFrame() * ratio;
    this->mocap_data->setFramePos(cur_frame_num);
    this->sendProgress(false);
    this->mocap_data->getOneFrame(this->cur_pose_joints, this->scene->getRawExMat());
}

void mGLWidget::tempPausePose() {
    if (this->is_has_pose) {
        this->temp_pose_state = 0; // temp stop
    }
}
void mGLWidget::tempStartPose() {
    if (this->is_has_pose) {
        this->temp_pose_state = 1; // temp start
    }
}
