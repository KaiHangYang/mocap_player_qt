#include "mGLWidget.h"
#include <QtWidgets>
#include <QDebug>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "mRotateUtils.h"
#include <opencv2/highgui/highgui.hpp>
#include "mVisual.h"

mGLWidget::mGLWidget(QWidget * parent, QGLFormat gl_format, int wnd_width, int wnd_height) : QGLWidget(gl_format, parent) {
    this->wnd_width = wnd_width;
    this->wnd_height = wnd_height;
    // set cam_in_mat cam_ex_mat and is_ar here
    this->cur_capture_sum = 0;
    this->cur_capture_view_mats = std::vector<glm::mat4>();
    this->is_set_capture_frame = false;
    this->is_set_capture_all_frames = false;
    this->cur_capture_type = 0; // global


    this->is_with_floor = true;
    this->pose_state = -1;
    this->temp_pose_state = 1;
    this->is_has_pose = false;
    this->cur_pose_joints = std::vector<glm::vec3>();
    this->pose_change_step = 200;

    this->is_ar = true;
    this->cam_in_mat = glm::mat4({1500.172, 0, this->wnd_width / 2, 0, 0, 1500.837, this->wnd_height / 2, 0, 0, 0, 1, 0, 0, 0, 0, 1});
    this->cam_ex_mat = glm::mat4({0.000575281, 0.06160985, -0.9981001, 221.3543, 0.2082146, -0.9762325, -0.06013997, 659.87, -0.978083, -0.2077844, -0.01338968, 3644.688, 0,
                                  0, 0, 1});

//    this->is_ar = false;
//    this->cam_in_mat = glm::transpose(glm::perspective(glm::radians(45.f), (float)this->wnd_width / this->wnd_height, 0.01f, 1000000.f));
//    this->cam_ex_mat = glm::transpose(glm::lookAt(glm::vec3(0, 10.f, 300.f), glm::vec3(0, 10.f, 0), glm::vec3(0, 1, 0)));
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
    this->scene->moveCamera(1, event);
}

void mGLWidget::mouseMoveEvent(QMouseEvent *event) {
    mCamRotate::mouse_move_callback(event);
    this->scene->moveCamera(2, event);
}
void mGLWidget::mouseDoubleClickEvent(QMouseEvent *event) {
    emit doubleClickPoseToggleSignal();
}
void mGLWidget::keyPressEvent(QKeyEvent *event) {
//    switch (event->key()) {
//        case Qt::Key_W:
//            // go up
//            this->scene->moveCamera(2);
//            break;
//        case Qt::Key_S:
//            // go down
//            this->scene->moveCamera(-2);
//            break;
//        case Qt::Key_A:
//            this->scene->moveCamera(1);
//            break;
//        case Qt::Key_D:
//            this->scene->moveCamera(-1);
//            break;
//    }
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
    if (this->is_with_floor) {
        glClearColor(0.4627450980392157f, 0.5882352941176471f, 0.8980392156862745f, 1.0f);
    }
    else {
        glClearColor(0.5f, 0.5f, 0.5f, 1.f);
    }
    this->draw();
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

void mGLWidget::setCurFollowVec(glm::vec3 cur_follow_vec) {
    this->scene->setCurFollowVec(cur_follow_vec);
}

glm::vec3 mGLWidget::getCurFollowVec() {
    return this->scene->getCurFollowVec();
}
void mGLWidget::setFollowPerson(bool is_follow) {
    this->scene->setFollowPerson(is_follow);
}
void mGLWidget::setFocusOnPerson(bool is_focus) {
    this->scene->setFocusOnCenter(is_focus);
}

void mGLWidget::getSplittedCameras(int camera_num, std::vector<glm::vec3> &splitted_cameras) {
    this->scene->getSplittedCameras(camera_num, splitted_cameras);
}
void mGLWidget::setFollowDefault() {
    this->scene->setFollowDefault();
}

void mGLWidget::captureFrame(const std::vector<glm::vec3> & view_vecs) {

    if (view_vecs.size() == 0) {
        this->cur_capture_view_vecs = std::vector<glm::vec3>({this->getCurFollowVec()});
    }
    else {
        this->cur_capture_view_vecs = view_vecs;
    }
    this->cur_capture_type = 1;
    this->cur_capture_sum = 0;
    this->tempPausePose();

    this->is_set_capture_frame = true;
}
void mGLWidget::captureFrame(const std::vector<glm::mat4> & view_mats) {
    if (view_mats.size() == 0) {
        this->cur_capture_view_mats = std::vector<glm::mat4>({this->getCurExMat()});
    }
    else {
        this->cur_capture_view_mats = view_mats;
    }
    this->cur_capture_type = 0;
    this->cur_capture_sum = 0;
    this->tempPausePose();

    this->is_set_capture_frame = true;
}

void mGLWidget::resetCapture() {
    if (this->is_set_capture_all_frames) {
        if (this->cur_capture_type == 1) {
            this->captureFrame(this->cur_capture_view_vecs);
        }
        else if (this->cur_capture_type == 0){
            this->captureFrame(this->cur_capture_view_mats);
        }
    }
    else {
        this->is_set_capture_frame = false;
        this->cur_capture_view_mats.clear();
        this->cur_capture_view_vecs.clear();
        this->cur_capture_sum = 0;
        this->cur_capture_type = 0;
    }
}

void mGLWidget::captureAllFrames(const std::vector<glm::vec3> &view_vecs) {
    this->cur_capture_type = 1;
    this->is_set_capture_all_frames = true;
    this->captureFrame(view_vecs);
}

void mGLWidget::captureAllFrames(const std::vector<glm::mat4> &view_mats) {
    this->cur_capture_type = 0;
    this->is_set_capture_all_frames = true;
    this->captureFrame(view_mats);
}

void mGLWidget::stopCapture() {
    if (this->is_set_capture_all_frames) {
        this->is_set_capture_all_frames = false;
        this->resetCapture();
    }
}

void mGLWidget::setUseFloor(bool is_with_floor) {
    this->scene->setFloor(is_with_floor);
    this->is_with_floor = is_with_floor;
}
/*************** Implementation of slots *****************/
void mGLWidget::changePoseFile(QString & file_name, int cur_dataset_num) {
    if (this->mocap_reader.parse(file_name, cur_dataset_num, this->mocap_data)) {
        // Get the first frame pose
        this->is_has_pose = true;
        this->pose_state = 0;
        this->sendProgress(true);
        this->mocap_data->getOneFrame(this->cur_pose_joints, 0.0);
    }
    else {
        this->is_has_pose = false;
        this->pose_state = -1;
    }
}

void mGLWidget::togglePose() {
    if (this->is_has_pose) {
        if (this->pose_state == 1) {
            this->stopPose();
        }
        else if (this->pose_state == 0){
            this->startPose();
        }
    }
}
void mGLWidget::startPose() {
    this->pose_state = 1;
}
void mGLWidget::stopPose() {
    this->pose_state = 0;
}
void mGLWidget::resetPose() {
    if (this->is_has_pose) {
        this->pose_state = 0;
        this->mocap_data->resetCounter();
        this->sendProgress(false);
        // Reset to the first frame pose
        this->mocap_data->getOneFrame(this->cur_pose_joints, 0.0);
    }
}

void mGLWidget::setPose(float ratio) {
    int cur_frame_num = this->mocap_data->getTotalFrame() * ratio;
    this->mocap_data->setFramePos(cur_frame_num);
    this->sendProgress(false);
    this->mocap_data->getOneFrame(this->cur_pose_joints, 0.0);
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

void mGLWidget::setPoseChangeStep(float change_step) {
    this->pose_change_step = change_step;
}

void mGLWidget::draw() {
    glm::mat4 cur_ex_r_mat, cur_ex_t_mat, cur_rotate_mat;
    this->scene->getCurExMat(cur_ex_r_mat, cur_ex_t_mat);
    cur_rotate_mat = mCamRotate::getRotateMat(this->wnd_width, this->wnd_height, cur_ex_r_mat, this->scene->m_rotate_dir);
    this->scene->rotateCamrea(cur_rotate_mat);

    if (this->pose_state == 1 && this->temp_pose_state == 1) {
        this->sendProgress(false);
        std::vector<glm::vec3> tmp_pose_joints;
        bool result = this->mocap_data->getOneFrame(tmp_pose_joints, this->pose_change_step);
        if (!result) {
            // read finished then read the next file
            emit changePoseFileSignal();
        }
        else {
            this->cur_pose_joints = tmp_pose_joints;
            // if use capture all, here I need to set flags for it.
        }
        if (this->is_set_capture_all_frames) {
            this->tempPausePose();
        }
    }

    // Just handle the frame capture of only one
    if (this->is_set_capture_frame) {
        if ((this->cur_capture_type == 0 && this->cur_capture_sum < this->cur_capture_view_mats.size()) || (this->cur_capture_type == 1 && this->cur_capture_sum < this->cur_capture_view_vecs.size())) {
            std::vector<glm::vec2> labels_2d;
            std::vector<glm::vec3> labels_3d;

            if (this->cur_capture_type == 0) {
                this->scene->render(this->cur_pose_joints, this->cur_capture_view_mats[this->cur_capture_sum]);
                this->scene->getLabelsFromFrame(this->cur_pose_joints, this->cur_capture_view_mats[this->cur_capture_sum], labels_2d, labels_3d);
            }
            else if (this->cur_capture_type == 1) {
                this->scene->render(this->cur_pose_joints, this->cur_capture_view_vecs[this->cur_capture_sum]);
                this->scene->getLabelsFromFrame(this->cur_pose_joints, this->cur_capture_view_vecs[this->cur_capture_sum], labels_2d, labels_3d);
            }

            cv::Mat captured_img;
            this->swapBuffers(); // Important to capture frames
            this->scene->captureFrame(captured_img);
//            mVTools::drawLines(captured_img, labels_2d);
            int cur_frame_num = this->mocap_data->getCurFrame();
            emit saveCapturedFrameSignal(captured_img, labels_2d, labels_3d, cur_frame_num, this->cur_capture_sum);
            this->cur_capture_sum++;
            return;
        }
        else {
            this->resetCapture();
            this->tempStartPose();
        }
    }

    this->scene->render(cur_pose_joints, glm::mat4(0.f));
}
