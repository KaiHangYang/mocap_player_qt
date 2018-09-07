#include "mGLWidget.h"
#include <QtWidgets>
#include <QDebug>
#include <climits>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "mRotateUtils.h"
#include <opencv2/highgui/highgui.hpp>
#include "mVisual.h"

#include "mRenderParameters.h"
#include "mSynthesisPaint.h"
#include "mIKOpt.h"

mGLWidget::mGLWidget(QWidget * parent, QGLFormat gl_format, int wnd_width, int wnd_height) : QGLWidget(gl_format, parent) {
    this->wnd_width = wnd_width;
    this->wnd_height = wnd_height;
    // set cam_in_mat cam_ex_mat and is_ar here
    this->cur_capture_cameras = std::vector<const mCamera *>();

    this->cur_pose_joints = std::vector<glm::vec3>();
    this->pose_change_step = 200;
    this->pose_jitter_range = 0;
    this->pose_angle_jitter_range = 0;

    /*********** State Parameters ***********/
    this->is_set_capture_frame = false;
    this->is_capture_all = false;

    this->pose_state = -1;
    this->temp_pose_state = 1;

    this->use_shading = true;
    this->is_with_floor = true;
    this->is_changing_pose = false;

    this->is_has_pose = false;
    this->is_ar = mRenderParams::m_is_ar;
    /****************************************/

    if (mRenderParams::m_camera_type == 0) {
        this->cam_in_mat = mRenderParams::m_cam_in_mat_perspective;
    }
    else {
        this->cam_in_mat = mRenderParams::m_cam_in_mat_ortho;
    }

    this->cam_ex_mat = mRenderParams::m_cam_ex_mat;

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

    this->scene = new mSceneUtils(this->VAO, this->core_func, this->wnd_width, this->wnd_height, this->cam_in_mat, this->cam_ex_mat, mRenderParams::m_camera_type, this->is_with_floor, this->use_shading, this->is_ar);
    glViewport(0, 0, this->wnd_width, this->wnd_height);
    glClearColor(0.4627450980392157f, 0.5882352941176471f, 0.8980392156862745f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    // Use the anti-alias

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
    if (this->is_changing_pose && event->button() == Qt::RightButton) {
        glm::vec2 clicked_pos(event->x(), event->y());
        this->cur_click_pos = clicked_pos;
        int index = this->getClickJointIndexOnCurPose(clicked_pos);
        this->cur_selected_joint_index = index;
    }

}

void mGLWidget::mouseMoveEvent(QMouseEvent *event) {
    mCamRotate::mouse_move_callback(event);
    this->scene->moveCamera(2, event);
}
void mGLWidget::mouseDoubleClickEvent(QMouseEvent *event) {
    if (!this->is_changing_pose) {
        emit doubleClickPoseToggleSignal();
    }
}
void mGLWidget::keyPressEvent(QKeyEvent *event) {
    float move_dert = 8.0;
    if (this->is_changing_pose) {
        if (event->key() == Qt::Key_W || event->key() == Qt::Key_S || event->key() == Qt::Key_A || event->key() == Qt::Key_D || event->key() == Qt::Key_Q || event->key() == Qt::Key_E) {
            float alpha_move=0, beta_move=0, gama_move=0;
            switch (event->key()) {
                case Qt::Key_W:
                    alpha_move = move_dert / this->wnd_width;
                    break;
                case Qt::Key_S:
                    alpha_move = -move_dert / this->wnd_width;
                    break;
                case Qt::Key_A:
                    beta_move = move_dert / this->wnd_width;
                    break;
                case Qt::Key_D:
                    beta_move = -move_dert / this->wnd_width;
                    break;
                case Qt::Key_Q:
                    gama_move = move_dert / this->wnd_width;
                    break;
                case Qt::Key_E:
                    gama_move = -move_dert / this->wnd_width;
                    break;
            }

            int angle_index = mPoseDef::angle_index_from_joint[this->cur_selected_joint_index];
            float dert_alpha = alpha_move * M_PI;
            float dert_beta = beta_move * M_PI;
            float dert_gama = gama_move * M_PI;

            this->cur_pose_angles[3*angle_index + 0] += dert_alpha;
            this->cur_pose_angles[3*angle_index + 1] += dert_beta;
            this->cur_pose_angles[3*angle_index + 2] += dert_gama;

            std::vector<double> tmp_joints = mIKOpt::points_from_angles<double>(&this->cur_pose_angles[0], this->cur_pose_bonelengths);
            for (int i = 0; i < this->cur_pose_joints.size(); ++i) {
                this->cur_pose_joints[i] = glm::vec3(tmp_joints[3*i+0], tmp_joints[3*i+1], tmp_joints[3*i+2]) + this->cur_pose_joint_root;
            }
        }
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
    if (this->is_with_floor) {
        glClearColor(0.4627450980392157f, 0.5882352941176471f, 0.8980392156862745f, 1.0f);
    }
    else {
        glClearColor(mRenderParams::mBgColor.x, mRenderParams::mBgColor.y, mRenderParams::mBgColor.z, 1.f);
    }
    this->draw();
}

int mGLWidget::getClickJointIndexOnCurPose(glm::vec2 click_pos) {
    int min_index = 0;
    if (this->is_has_pose) {
        std::vector<glm::vec2> joints_2d;
        this->scene->get2DJointsOnCurCamera(this->cur_pose_joints, joints_2d);

        float min_value = FLT_MAX;
        float tmp_value;
        for (int i = 0; i < joints_2d.size(); ++i) {
            tmp_value = glm::length(click_pos - joints_2d[i]);
            if (tmp_value < min_value) {
                min_value = tmp_value;
                min_index = i;
            }
        }
    }

    return min_index;
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
void mGLWidget::setCurInMat(glm::mat4 proj_mat) {
    this->scene->setCurInMat(proj_mat);
}
void mGLWidget::setCurCamera(const mCamera *camera) {
    this->scene->setCurCamera(camera);
}
const mCamera * mGLWidget::getCurCamera() {
    return this->scene->getCurCamera();
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
bool mGLWidget::getFollowPerson() {
    return this->scene->getFollowPerson();
}
bool mGLWidget::getFocusOnPerson() {
    return this->scene->getFocusOnCenter();
}
void mGLWidget::getSplittedCameras(int camera_num, std::vector<glm::vec3> &splitted_cameras) {
    this->scene->getSplittedCameras(camera_num, splitted_cameras);
}
void mGLWidget::setPoseChangeStep(float change_step) {
    this->pose_change_step = change_step;
}
void mGLWidget::setUseFloor(bool is_with_floor) {
    this->is_with_floor = is_with_floor;
}
void mGLWidget::setJitter(float jitter_size) {
    this->pose_jitter_range = jitter_size;
}
void mGLWidget::setAngleJitter(float jitter_size) {
    this->pose_angle_jitter_range = jitter_size;
}
void mGLWidget::setUseShading(bool use_shading) {
    this->use_shading = use_shading;
}
void mGLWidget::setVerticalAngle(float angle) {
    this->scene->setVerticalAngle(angle);
}

void mGLWidget::setVisualizeCameras(std::vector<const mCamera *> cameras_arr) {
    this->cur_visualization_cameras = cameras_arr;
}

int mGLWidget::getCurCameraType() {
    return this->scene->getCurCameraType();
}
void mGLWidget::setCurCameraType(int camera_type) {
    this->scene->setCurCameraType(camera_type);
}

std::vector<glm::vec3> mGLWidget::getCurPoseJoints() {
    if (this->is_has_pose) {
        return this->cur_pose_joints;
    }
    else {
        return std::vector<glm::vec3>();
    }
}

void mGLWidget::setCurPoseJoints(const std::vector<glm::vec3> &cur_pose) {
    if (this->is_has_pose) {
        this->cur_pose_joints = cur_pose;
    }
}

void mGLWidget::setIsChangingPose(bool is_changing, std::vector<glm::vec3> cur_pose) {
    if (this->is_has_pose) {
        this->is_changing_pose = is_changing;
        if (this->is_changing_pose) {
            if (cur_pose.size() == 0) {
                cur_pose = this->cur_pose_joints;
            }
            // If I begin to change the pose, I will get the angle representation of the pose first
            this->cur_pose_bonelengths = std::vector<double>(mPoseDef::num_of_bonelength, 0);
            std::vector<double> cur_pose_joints_3d(3*mPoseDef::num_of_joints, 0);

            for (int i = 0; i < mPoseDef::num_of_bones; ++i) {
                glm::vec2 cur_bone = mPoseDef::bones_indices[i];
                float cur_length = glm::length(cur_pose[cur_bone.x] - cur_pose[cur_bone.y]);
                if (this->cur_pose_bonelengths[mPoseDef::bones_length_index[i]] == 0) {
                    this->cur_pose_bonelengths[mPoseDef::bones_length_index[i]] = cur_length;
                }
                else {
                    this->cur_pose_bonelengths[mPoseDef::bones_length_index[i]] += cur_length;
                    this->cur_pose_bonelengths[mPoseDef::bones_length_index[i]] /= 2;
                }
            }
            for (int i = 0; i < mPoseDef::num_of_joints; ++i) {
                glm::vec3 cur_rel_joint = cur_pose[i] - cur_pose[mPoseDef::root_of_joints];
                cur_pose_joints_3d[3*i + 0] = cur_rel_joint.x;
                cur_pose_joints_3d[3*i + 1] = cur_rel_joint.y;
                cur_pose_joints_3d[3*i + 2] = cur_rel_joint.z;
            }
            this->cur_pose_angles = mIKOpt::optimizeIK(cur_pose_joints_3d, this->cur_pose_bonelengths);
            this->_cur_pose_angles = this->cur_pose_angles;
            this->cur_pose_joint_root = cur_pose[mPoseDef::root_of_joints];
        }
    }
}

void mGLWidget::resetChangingPose() {
    this->cur_pose_angles = this->_cur_pose_angles;
    std::vector<double> tmp_joints = mIKOpt::points_from_angles<double>(&this->cur_pose_angles[0], this->cur_pose_bonelengths);
    for (int i = 0; i < this->cur_pose_joints.size(); ++i) {
        this->cur_pose_joints[i] = glm::vec3(tmp_joints[3*i+0], tmp_joints[3*i+1], tmp_joints[3*i+2]) + this->cur_pose_joint_root;
    }
}

void mGLWidget::captureAllFrames(std::vector<const mCamera *> cameras) {
    this->is_capture_all = true;
    this->captureFrame(cameras);
}
void mGLWidget::captureFrame(std::vector<const mCamera *> cameras) {
    this->cur_capture_cameras.clear();
    if (cameras.size() == 0) {
        this->cur_capture_cameras.push_back(const_cast<const mCamera *>(this->scene->getCurCamera()));
    }
    else {
        this->cur_capture_cameras = cameras;
    }
    this->is_set_capture_frame = true;
}
void mGLWidget::resetCapture() {
    this->is_set_capture_frame = false;
    this->cur_capture_cameras.clear();
}

void mGLWidget::stopCaptureAll() {
    this->is_capture_all = false;
    this->resetCapture();
}

/*************** Implementation of slots *****************/
void mGLWidget::changePoseFile(QString & file_name, int cur_dataset_num) {
    if (this->mocap_reader.parse(file_name, cur_dataset_num, this->mocap_data)) {
        // Get the first frame pose
        this->is_has_pose = true;
        this->pose_state = 0;
        this->sendProgress(true);
        this->mocap_data->getOneFrame(this->cur_pose_joints, this->cur_pose_joints_raw, 0.0, this->pose_jitter_range, this->pose_angle_jitter_range);
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

void mGLWidget::tempPausePose() {
    this->temp_pose_state = 0;
}

void mGLWidget::tempStartPose() {
    this->temp_pose_state = 1;
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
        this->mocap_data->getOneFrame(this->cur_pose_joints, this->cur_pose_joints_raw, 0.0, this->pose_jitter_range, this->pose_angle_jitter_range);
    }
}

void mGLWidget::setPose(float ratio) {
    int cur_frame_num = this->mocap_data->getTotalFrame() * ratio;
    this->mocap_data->setFramePos(cur_frame_num);
    this->sendProgress(false);
    this->mocap_data->getOneFrame(this->cur_pose_joints, this->cur_pose_joints_raw, 0.0, this->pose_jitter_range, this->pose_angle_jitter_range);
}

void mGLWidget::draw() {
    /*************** Code for changing the pose *****************/
    // the temp_pose_state is for the prograss bar control
    if (this->pose_state == 1 && this->temp_pose_state) {
        this->sendProgress(false);
        std::vector<glm::vec3> tmp_pose_joints;
        bool result = this->mocap_data->getOneFrame(tmp_pose_joints, this->cur_pose_joints_raw, this->pose_change_step, this->pose_jitter_range, this->pose_angle_jitter_range);
        if (!result) {
            // read finished then read the next file
            emit changePoseFileSignal();
        }
        else {
            this->cur_pose_joints = tmp_pose_joints;
            // Then I need to update the pose center
            this->scene->setPoseCenter(this->cur_pose_joints[this->cur_pose_joints.size() - 1]);

        }
    }
    /*************************************************************/

    /*********************** Code for render and captures ********************/
    if (this->is_set_capture_frame) {

        for (int cam_num = 0; cam_num < this->cur_capture_cameras.size(); ++cam_num) {
            std::vector<glm::vec2> labels_2d;
            std::vector<glm::vec3> labels_3d;

            std::vector<glm::vec2> labels_2d_raw;
            std::vector<glm::vec3> labels_3d_raw;

            std::vector<glm::vec3> adjusted_pose_joints;
            std::vector<glm::vec3> adjusted_pose_joints_raw;

            // When the camera is ortho camera, I adjust the position of the joints to make the pose more like the one in the perspective mode.
            adjusted_pose_joints = this->scene->adjustPoseAccordingToCamera(this->cur_pose_joints, this->cur_capture_cameras[cam_num]);
            adjusted_pose_joints_raw = this->scene->adjustPoseAccordingToCamera(this->cur_pose_joints_raw, this->cur_capture_cameras[cam_num]);

            this->scene->render(this->cur_pose_joints, adjusted_pose_joints, this->cur_capture_cameras[cam_num]);
            this->scene->getLabelsFromFrame(this->cur_pose_joints, adjusted_pose_joints, this->cur_capture_cameras[cam_num], labels_2d, labels_3d);
            if (this->is_ar) {
                this->scene->getLabelsFromFrame(this->cur_pose_joints_raw, adjusted_pose_joints_raw, this->cur_capture_cameras[cam_num], labels_2d_raw, labels_3d_raw);
            }

            cv::Mat captured_img;
            // After all the part are renderend

            this->core_func->glFinish();
            this->makeCurrent();
            this->swapBuffers(); // Important to capture frames because the this->scene->captureFrame use the GL_FRONT as the frame img.
            this->scene->captureFrame(captured_img);

            int cur_frame_num = this->mocap_data->getCurFrame();
//            emit saveCapturedFrameSignal(captured_img, cur_frame_num, cam_num, "_gl");
//            emit saveCapturedLabelSignal(labels_2d, labels_3d, cur_frame_num, cam_num, false);
            if (this->is_ar) {
                emit saveCapturedLabelSignal(labels_2d_raw, labels_3d_raw, cur_frame_num, cam_num, true);
            }

            // Then Save the synthesised img
            cv::Mat synthesis_img(cv::Size(mRenderParams::cropTargetSize, mRenderParams::cropTargetSize), CV_8UC3, cv::Scalar(mRenderParams::mBgColor.x * 255, mRenderParams::mBgColor.y * 255, mRenderParams::mBgColor.z * 255));

            // Currently the labels_3d is the joints in the real world camera coordinate, and to get information from the raw 1024x1024 rendered img, the labels_2d need to be the joints in 1024x1024.
            mSynthesisPaint::drawSynthesisData(captured_img.ptr<unsigned char>(), glm::u32vec3(1024, 1024, 3), labels_2d, labels_3d, synthesis_img);
            emit saveCapturedFrameSignal(synthesis_img, cur_frame_num, cam_num, "");
            // NOTICE: the labels_2d is changed in the mSynthesisPaint::drawSynthesisData.
            emit saveCapturedLabelSignal(labels_2d, labels_3d, cur_frame_num, cam_num, false);
        }
        // This frame captured finished, if capture_all then continue, or reset capture.
        // The capture all is stop beyond this file
        if (!this->is_capture_all) {
            this->resetCapture();
        }
    }
    else {
        /******************* Code for handle the rotate and only in the free mode, this function is used ***************/
        // Cause the mouse event is handled here.
        glm::mat4 cur_ex_r_mat, cur_ex_t_mat, cur_rotate_mat;
        this->scene->getCurExMat(cur_ex_r_mat, cur_ex_t_mat);
        cur_rotate_mat = mCamRotate::getRotateMat(this->wnd_width, this->wnd_height, cur_ex_r_mat, this->scene->m_rotate_dir);
        this->scene->rotateCamera(cur_rotate_mat);
        /***************************************************************************************************************/
        std::vector<glm::vec3> adjusted_pose_joints;

        adjusted_pose_joints = this->scene->adjustPoseAccordingToCamera(this->cur_pose_joints);
        this->scene->render(this->cur_pose_joints, adjusted_pose_joints);

//        if (this->cur_pose_joints.size() != 0) {
//            cv::Mat captured_img;
//            std::vector<glm::vec2> labels_2d;
//            std::vector<glm::vec3> labels_3d;

//            this->scene->captureFrame(captured_img);
//            this->scene->getLabelsFromFrame(this->cur_pose_joints, adjusted_pose_joints, this->scene->getCurCamera(), labels_2d, labels_3d);

//            cv::Mat synthesis_img(cv::Size(1024, 1024), CV_8UC3, cv::Scalar(51, 51, 51));

//            mSynthesisPaint::drawSynthesisData(captured_img.ptr<unsigned char>(), glm::u32vec3(1024, 1024, 3), labels_2d, labels_3d, synthesis_img);
//            cv::imshow("captured_img", synthesis_img);
//            cv::waitKey(3);
//        }

        /******************************************/

        // the render will clear the color and depth bit, so I need to render the camera below
        if (!this->cur_visualization_cameras.empty()) {
            this->scene->renderCamerasPos(this->cur_visualization_cameras);
        }
    }
}
