#include "mSceneUtils.h"


#include <iostream>
#include <glm/gtx/component_wise.hpp>
#include <QDebug>
#include "mRenderParameters.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include "mPoseDefs.h"
#include <glm/gtx/string_cast.hpp>

static float move_step = 80.0;

mSceneUtils::mSceneUtils(QOpenGLVertexArrayObject * vao, QOpenGLFunctions_3_3_Core * core_func, int wnd_width, int wnd_height, glm::vec4 cam_in_vec, glm::mat4 cam_ex_mat, int camera_type, bool & is_with_floor, bool & use_shading, int pose_type):is_with_floor(is_with_floor), use_shading(use_shading) {
    this->wnd_width = wnd_width;
    this->wnd_height = wnd_height;

    this->VAO = vao;
    this->core_func = core_func;

    this->person_center_pos = glm::vec3(0.f);
    this->prev_mouse_pos = glm::vec2(-1.f);
    /******************************************************************/

    // The parameter maybe changed as reality make sure the ground_col and ground_row is even
    this->ground_col = 100;
    this->ground_row = 100;

    float target_model_size;


    target_model_size = 2 * 1;
    this->ground_size = 20.f;
    this->move_step_scale = 0.03f;
    this->m_move_dir[0] = 1;this->m_move_dir[1] = 1;this->m_move_dir[2] = 1;
    this->m_rotate_dir[0] = 1; this->m_rotate_dir[1] = 1;


    this->scene_shader = new mShader(mRenderParams::mPoseShaderFiles[0], mRenderParams::mPoseShaderFiles[1]);
    this->depth_shader = new mShader(mRenderParams::mDepthShaderFiles[0], mRenderParams::mDepthShaderFiles[1], mRenderParams::mDepthShaderFiles[2]);

    this->cur_camera = new mCamera(cam_in_vec, cam_ex_mat, camera_type, this->wnd_width, this->wnd_height);
    this->pose_model = new mPoseModel(this->VAO, this->core_func, this->scene_shader, this->depth_shader, target_model_size, this->use_shading, pose_type);

    this->initScene();

    // Read the camera mesh.
    this->camera_mesh = new mMeshReader(this->VAO, this->core_func);
    this->camera_mesh->addMesh(mPoseDef::model_base_dir + "camera.obj");
}
mSceneUtils::~mSceneUtils() {
    this->pose_model->~mPoseModel();
    this->scene_shader->~mShader();
    this->depth_shader->~mShader();
    this->cur_camera->~mCamera();
    this->camera_mesh->~mMeshReader();
}

void mSceneUtils::initScene() {
    // Use the same vao for rendering the shading
    this->VAO->bind();

    this->core_func->glGenBuffers(1, &this->ground_vbo);
    this->core_func->glGenBuffers(1, &this->ground_cbo);

    std::vector<GLfloat> vertexs_data = this->getGroundVertexs();
    std::vector<GLfloat> color_data = this->getGroundColor();

    this->array_size = vertexs_data.size() / 3;

    this->core_func->glBindBuffer(GL_ARRAY_BUFFER, this->ground_vbo);
    this->core_func->glBufferData(GL_ARRAY_BUFFER, vertexs_data.size() * sizeof(GLfloat), &vertexs_data[0], GL_STATIC_DRAW);

    this->core_func->glBindBuffer(GL_ARRAY_BUFFER, this->ground_cbo);
    this->core_func->glBufferData(GL_ARRAY_BUFFER, color_data.size() * sizeof(GLfloat), &color_data[0], GL_STATIC_DRAW);

    /************************ Handel shadow displayment ****************************/
    this->shadow_fbo = std::vector<GLuint>(mRenderParams::mLightSum, 0);
    this->shadow_tbo = std::vector<GLuint>(mRenderParams::mLightSum, 0);

    this->core_func->glEnable(GL_DEPTH_TEST);
    this->core_func->glGenFramebuffers(mRenderParams::mLightSum, &this->shadow_fbo[0]);
    this->core_func->glGenTextures(mRenderParams::mLightSum, &this->shadow_tbo[0]);

    for (int light_num = 0; light_num < mRenderParams::mLightSum; ++light_num) {
        this->core_func->glBindTexture(GL_TEXTURE_CUBE_MAP, this->shadow_tbo[light_num]);
        for (int i = 0; i < 6; ++i) {
            this->core_func->glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, mRenderParams::mShadowWndWidth, mRenderParams::mShadowWndHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        }

        this->core_func->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        this->core_func->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        this->core_func->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        this->core_func->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        this->core_func->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        this->core_func->glBindFramebuffer(GL_FRAMEBUFFER, this->shadow_fbo[light_num]);
        this->core_func->glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->shadow_tbo[light_num], 0);
        // Tell OpenGL not to render color buffer
        this->core_func->glDrawBuffer(GL_NONE);
        this->core_func->glReadBuffer(GL_NONE);

        if (this->core_func->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            qDebug() << "(mSceneUtils.cpp) Shadow Framebuffer is not complete!";
            exit(-1);
        }
        this->core_func->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    /*******************************************************************************/
    this->VAO->release();
}
std::vector<GLfloat> mSceneUtils::getGroundColor() {
    std::vector<GLfloat> result_gd(this->ground_col * this->ground_row * 18, 0);
    GLfloat * r_ptr = &result_gd[0];
    float color_table[2][3] = {
        {0.4470588235294118, 0.4745098039215686, 0.611764705882353},
        {0.8784313725490196, 0.9098039215686274, 1}
    };

    int start_index = 0;
    int cur_index;

    for (int i = 0; i < this->ground_row; ++i) {
        start_index = !start_index;
        cur_index = start_index;
        for (int j = 0; j < this->ground_col; ++j) {
            *(r_ptr++) = color_table[cur_index][0];
            *(r_ptr++) = color_table[cur_index][1];
            *(r_ptr++) = color_table[cur_index][2];

            *(r_ptr++) = color_table[cur_index][0];
            *(r_ptr++) = color_table[cur_index][1];
            *(r_ptr++) = color_table[cur_index][2];

            *(r_ptr++) = color_table[cur_index][0];
            *(r_ptr++) = color_table[cur_index][1];
            *(r_ptr++) = color_table[cur_index][2];

            *(r_ptr++) = color_table[cur_index][0];
            *(r_ptr++) = color_table[cur_index][1];
            *(r_ptr++) = color_table[cur_index][2];

            *(r_ptr++) = color_table[cur_index][0];
            *(r_ptr++) = color_table[cur_index][1];
            *(r_ptr++) = color_table[cur_index][2];

            *(r_ptr++) = color_table[cur_index][0];
            *(r_ptr++) = color_table[cur_index][1];
            *(r_ptr++) = color_table[cur_index][2];

            cur_index = !cur_index;
        }
    }

    return result_gd;
}
std::vector<GLfloat> mSceneUtils::getGroundVertexs() {
    std::vector<GLfloat> result_gd(this->ground_col*this->ground_row * 18, 0);
    
    int row_from = -1 * this->ground_row / 2;
    int row_end = -row_from;

    int col_from = -1 * this->ground_col / 2;
    int col_end = -col_from;

    GLfloat * r_ptr = &result_gd[0];

    for (int i = row_from; i < row_end; ++i) {
        for (int j = col_from; j < col_end; ++j) {

            *(r_ptr++) = j * this->ground_size;
            *(r_ptr++) = 0.0f;
            *(r_ptr++) = (i+1) * this->ground_size;

            *(r_ptr++) = (j+1) * this->ground_size;
            *(r_ptr++) = 0.0f;
            *(r_ptr++) = i * this->ground_size;

            *(r_ptr++) = j * this->ground_size;
            *(r_ptr++) = 0.0f;
            *(r_ptr++) = i * this->ground_size;

            *(r_ptr++) = j * this->ground_size;
            *(r_ptr++) = 0.0f;
            *(r_ptr++) = (i+1) * this->ground_size;

            *(r_ptr++) = (j+1) * this->ground_size;
            *(r_ptr++) = 0.0f;
            *(r_ptr++) = (i+1) * this->ground_size;

            *(r_ptr++) = (j+1) * this->ground_size;
            *(r_ptr++) = 0.0f;
            *(r_ptr++) = i * this->ground_size;
        }
    }

    return result_gd;
}

void mSceneUtils::moveCamera(int move_type, QMouseEvent * event) {
    glm::vec3 total_move_step(0.f);
    if (std::abs(move_type) == 1 && event->buttons() & Qt::MiddleButton) {
        // set the prev_mouse_pos
        this->prev_mouse_pos = glm::vec2(event->pos().x(), event->pos().y());
    }
    else if (std::abs(move_type) == 2 && event->buttons() & Qt::MiddleButton) {
        glm::vec2 cur_mouse_pos = glm::vec2(event->pos().x(), event->pos().y());
        glm::vec2 move_dert = (cur_mouse_pos - this->prev_mouse_pos);
        total_move_step.x = -move_dert.x * this->move_step_scale * 33.3333f * this->m_move_dir[0];
        total_move_step.y = move_dert.y * this->move_step_scale * 33.3333f * this->m_move_dir[1];

        this->prev_mouse_pos = cur_mouse_pos;
    }
    else if (std::abs(move_type) == 3) {
        float sign = move_type > 0?1.f:-1.f;
        total_move_step.z = sign * move_step * this->move_step_scale * 4.f * this->m_move_dir[2];
    }
    this->cur_camera->moveCamera(total_move_step, this->person_center_pos);
}
void mSceneUtils::rotateCamera(glm::mat4 rotate_mat) {
    this->cur_camera->rotateCamera(rotate_mat);
}

void mSceneUtils::setCurCamera(const mCamera * camera) {
    *(this->cur_camera) = *camera;
}
const mCamera* mSceneUtils::getCurCamera() {
    return this->cur_camera;
}
void mSceneUtils::setCurExMat(glm::mat4 cur_ex_mat) {
    this->cur_camera->setViewMat(cur_ex_mat);
}
glm::mat4 mSceneUtils::getCurExMat(glm::mat4 & cam_ex_r_mat, glm::mat4 & cam_ex_t_mat) {
    return this->cur_camera->getViewMat(this->person_center_pos, &cam_ex_r_mat, &cam_ex_t_mat);
}
glm::mat4 mSceneUtils::getCurExMat() {
    return this->cur_camera->getViewMat(this->person_center_pos);
}

glm::vec3 mSceneUtils::getCurFollowVec() {
    return this->cur_camera->getViewVec();
}
void mSceneUtils::setCurFollowVec(glm::vec3 cur_follow_vec) {
    this->cur_camera->setViewVec(cur_follow_vec);
}
void mSceneUtils::setFollowPerson(bool is_follow) {
    this->cur_camera->setFollow(is_follow, this->person_center_pos);
}
void mSceneUtils::setFocusOnCenter(bool is_focus_on_center) {
    this->cur_camera->setFocus(is_focus_on_center);
}

bool mSceneUtils::getFollowPerson() {
    return this->cur_camera->getFollow();
}
bool mSceneUtils::getFocusOnCenter() {
    return this->cur_camera->getFocus();
}

int mSceneUtils::getCurCameraType() {
    return this->cur_camera->getCameraType();
}
void mSceneUtils::setCurCameraType(int camera_type) {
    this->cur_camera->setCameraType(camera_type);
}

void mSceneUtils::captureFrame(cv::Mat & cur_frame) {
    this->core_func->glReadBuffer(GL_FRONT);
    if (cur_frame.size().height != this->wnd_height || cur_frame.size().width != this->wnd_width) {
        cur_frame = cv::Mat(this->wnd_height, this->wnd_width, CV_8UC3);
    }
    this->core_func->glReadPixels(0, 0, this->wnd_width, this->wnd_height, GL_BGR, GL_UNSIGNED_BYTE, cur_frame.ptr<unsigned char>());
    cv::flip(cur_frame, cur_frame, 0);
}

void mSceneUtils::getSplittedCameras(int camera_num, std::vector<glm::mat4> &splitted_cameras) {
    this->cur_camera->getSplittedCameras(camera_num, this->person_center_pos, splitted_cameras);
}
void mSceneUtils::getSplittedCameras(int camera_num, std::vector<glm::vec3> &splitted_cameras) {
    this->cur_camera->getSplittedCameras(camera_num, this->person_center_pos, splitted_cameras);
}
// This funciont can only be used in the follow mode
void mSceneUtils::setVerticalAngle(float angle) {
    this->cur_camera->setVerticalAngle(angle, this->person_center_pos);
}
void mSceneUtils::setPoseCenter(glm::vec3 pose_center) {
    this->person_center_pos = pose_center;
}

void mSceneUtils::setCurInMat(glm::vec4 proj_vec) {
    this->cur_camera->setProjMat(proj_vec);
}


// TODO: The 3D view coordinate labels need to be checked.
//       THe 2D is right, after visualized.

void mSceneUtils::getJointsInViewCoord_64f(const std::vector<glm::vec3> & joints_3d_raw, const mCamera * camera, std::vector<glm::f64vec2> & labels_2d_f64, std::vector<glm::f64vec3> & labels_3d_f64) {
    glm::f64mat4 view_mat(camera->getViewMat(this->person_center_pos));
    glm::f64mat4 proj_mat(camera->getProjMat());

    labels_2d_f64 = std::vector<glm::f64vec2>(joints_3d_raw.size());
    labels_3d_f64 = std::vector<glm::f64vec3>(joints_3d_raw.size());
    for (int i = 0; i < joints_3d_raw.size(); ++i) {
        glm::f64vec4 cur_2d = proj_mat * view_mat * glm::f64vec4(joints_3d_raw[i], 1.0);
        cur_2d /= cur_2d.w;
        labels_2d_f64[i] = glm::f64vec2(this->wnd_width * (cur_2d.x + 1.0) / 2.0 , this->wnd_height * (1.0 - cur_2d.y) / 2.0);
        labels_3d_f64[i] = glm::f64vec3(view_mat * glm::f64vec4(joints_3d_raw[i], 1.0));
    }
}

void mSceneUtils::_getLabelsFromFrame(const std::vector<glm::vec3> & joints_raw, const std::vector<glm::vec3> & joints_adjusted, const glm::mat4 & view_mat, const glm::mat4 & proj_mat, std::vector<glm::vec2> & labels_2d, std::vector<glm::vec3> & labels_3d) {
    if (joints_raw.size() != 0) {
        labels_2d = std::vector<glm::vec2>(joints_raw.size());
        labels_3d = std::vector<glm::vec3>(joints_raw.size());

        // Used for the "Current camera" Mode
        glm::mat4 cur_view_mat = view_mat;
        if (cur_view_mat == glm::mat4(0.f)) {
            cur_view_mat = this->getCurExMat();
        }
        glm::vec3 root_joint = glm::vec3(cur_view_mat * glm::vec4(joints_raw[joints_raw.size() - 1], 1.f));
        glm::mat4 cam_proj_mat = proj_mat;
        for (int i = 0; i < joints_raw.size(); ++i) {
            glm::vec4 cur_2d = cam_proj_mat * cur_view_mat * glm::vec4(joints_adjusted[i], 1.0);
            cur_2d /= cur_2d.w;
            labels_2d[i] = glm::vec2(this->wnd_width * (cur_2d.x + 1.f) / 2.f , this->wnd_height * (1.f - cur_2d.y) / 2.f);
            labels_3d[i] = glm::vec3(cur_view_mat * glm::vec4(joints_raw[i], 1.f)) - root_joint;

            // Cause the camera in the real word

            labels_3d[i].y *= -1;
            labels_3d[i].z *= -1;
        }
        mPoseDef::scalePose(labels_3d);
    }
}

void mSceneUtils::getLabelsFromFrame(const std::vector<glm::vec3> & joints_raw, const std::vector<glm::vec3> & joints_adjusted, const mCamera * camera, std::vector<glm::vec2> & labels_2d, std::vector<glm::vec3> & labels_3d) {
    // Joints is in the global coordinate
    glm::mat4 view_mat = camera->getViewMat(this->person_center_pos);
    glm::mat4 proj_mat = camera->getProjMat();
    this->_getLabelsFromFrame(joints_raw, joints_adjusted, view_mat, proj_mat, labels_2d, labels_3d);
}

void mSceneUtils::get2DJointsOnCurCamera(const std::vector<glm::vec3> &adjusted_joints_3d, std::vector<glm::vec2> &joints_2d) {
    joints_2d = std::vector<glm::vec2>(adjusted_joints_3d.size(), glm::vec2(0.f));

    glm::mat4 view_mat = this->cur_camera->getViewMat(this->person_center_pos);
    glm::mat4 proj_mat = this->cur_camera->getProjMat();

    for (int i = 0; i < adjusted_joints_3d.size(); ++i) {
        glm::vec4 cur_joint(adjusted_joints_3d[i], 1.0);
        cur_joint = proj_mat * view_mat * cur_joint;
        cur_joint /= cur_joint.w;
        joints_2d[i] = glm::vec2(this->wnd_width * (cur_joint.x + 1.f) / 2.f, this->wnd_height * (1.f - cur_joint.y) / 2.f);
    }
}

void mSceneUtils::_setDepthShaderUniforms(int light_num) {
    for (int i = 0; i < 6; ++i) {
        this->depth_shader->setVal(("shadow_mat["+std::to_string(i)+"]").c_str(), mRenderParams::mShadowTransforms[light_num][i]);
    }
    this->depth_shader->setVal("far_plane", mRenderParams::mShadowFarPlane);
    this->depth_shader->setVal("lightPos", mRenderParams::mLightPos[light_num]);

    this->depth_shader->setVal("model", glm::mat4(1.f));
}
void mSceneUtils::_setSceneShaderUnoforms(glm::mat4 model_mat, glm::mat4 view_mat, glm::mat4 proj_mat, bool is_use_shadow) {
    glm::mat4 view_r_mat = glm::mat4(glm::mat3(view_mat));
    glm::mat4 view_t_mat = glm::inverse(view_r_mat) * view_mat;
    this->scene_shader->setVal("use_shading", this->use_shading);
    this->scene_shader->setVal("renderType", 1);
    this->scene_shader->setVal("use_shadow", is_use_shadow);
    this->scene_shader->setVal("viewPos", glm::vec3(-view_t_mat[3][0], -view_t_mat[3][1], -view_t_mat[3][2]));
    this->scene_shader->setVal("projection", proj_mat);
    this->scene_shader->setVal("view", view_mat);
    this->scene_shader->setVal("model", model_mat);
    this->scene_shader->setVal("normMat", glm::transpose(glm::inverse(model_mat)));

    this->scene_shader->setVal("far_plane", mRenderParams::mShadowFarPlane);
    this->scene_shader->setVal("shadow_bias", mRenderParams::mBias);

    for (int light_num = 0; light_num < mRenderParams::mLightSum; ++light_num) {
        this->scene_shader->setVal(("pointLights[" + std::to_string(light_num) + "].ambient").c_str(), mRenderParams::mAmbient);
        this->scene_shader->setVal(("pointLights[" + std::to_string(light_num) + "].diffuse").c_str(), mRenderParams::mDiffuse);
        this->scene_shader->setVal(("pointLights[" + std::to_string(light_num) + "].specular").c_str(), mRenderParams::mSpecular);

        this->scene_shader->setVal(("pointLights[" + std::to_string(light_num) + "].position").c_str(), mRenderParams::mLightPos[light_num]);

        this->scene_shader->setVal(("depth_cube["+ std::to_string(light_num) + "]").c_str(), 1 + light_num);

        this->core_func->glActiveTexture(GL_TEXTURE1 + light_num);
        this->core_func->glBindTexture(GL_TEXTURE_CUBE_MAP, this->shadow_fbo[light_num]);
    }
}

void mSceneUtils::_drawFloor() {
    this->core_func->glBindBuffer(GL_ARRAY_BUFFER, this->ground_vbo);
    this->core_func->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    this->core_func->glEnableVertexAttribArray(0);

    this->core_func->glBindBuffer(GL_ARRAY_BUFFER, this->ground_cbo);
    this->core_func->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    this->core_func->glEnableVertexAttribArray(1);

    this->core_func->glDrawArrays(GL_TRIANGLES, 0, this->array_size);

    this->core_func->glDisableVertexAttribArray(0);
    this->core_func->glDisableVertexAttribArray(1);
}

// This function is used the make the pose in the ortho camera the same as the one in the perspective camera.
std::vector<glm::vec3> mSceneUtils::adjustPoseAccordingToCamera(std::vector<glm::vec3> joints_3d, const mCamera *camera) {
    std::vector<glm::vec3> result_joints_3d = joints_3d;

//    this->_beforeRender(joints_3d);
//    glm::mat4 cur_cam_ex_mat, cur_cam_in_mat;
//    int camera_type;
//    if (camera != nullptr) {
//        cur_cam_ex_mat = camera->getViewMat(this->person_center_pos);
//        cur_cam_in_mat = camera->getProjMat();
//        camera_type = camera->getCameraType();
//    }
//    else {
//        cur_cam_ex_mat = this->cur_camera->getViewMat(this->person_center_pos);
//        cur_cam_in_mat = this->cur_camera->getProjMat();
//        camera_type = this->cur_camera->getCameraType();
//    }

    // When the capture camera is ortho, then use the adjusted joints to render(Only used for renderring).
//    if (camera_type == 1) {
//        /****************** Change the joint position if the camera is ortho *******************/
//        /****************** This is why when I change the z of the camera, the size of the model will change. ****************/
//        for (int i = 0; i < joints_3d.size(); ++i) {
//            glm::vec4 cur_vertex(joints_3d[i], 1.0);
//            cur_vertex = glm::transpose(mRenderParams::m_cam_in_mat_perspective) * cur_cam_ex_mat * glm::mat4(1.f) * cur_vertex;
//            cur_vertex /= cur_vertex.w;
//            cur_vertex = glm::inverse(cur_cam_ex_mat) * glm::inverse(cur_cam_in_mat) * cur_vertex;
//            result_joints_3d[i] = glm::vec3(cur_vertex);
//        }
//    }

    return result_joints_3d;
}

void mSceneUtils::_beforeRender(const std::vector<glm::vec3> & points_3d) {
    // points_3d is in the global coordinate
    if (points_3d.size() == this->pose_model->num_of_joints) {
        this->setPoseCenter(points_3d[points_3d.size() - 1]);
    }
}


void mSceneUtils::render(std::vector<glm::vec3> points_3d_raw, std::vector<glm::vec3> points_3d, const mCamera * camera) {
    this->_beforeRender(points_3d_raw);
    glm::mat4 cur_cam_ex_mat, cur_cam_in_mat;
    int camera_type;
    if (camera != nullptr) {
        cur_cam_ex_mat = camera->getViewMat(this->person_center_pos);
        cur_cam_in_mat = camera->getProjMat();
        camera_type = camera->getCameraType();
    }
    else {
        cur_cam_ex_mat = this->cur_camera->getViewMat(this->person_center_pos);
        cur_cam_in_mat = this->cur_camera->getProjMat();
        camera_type = this->cur_camera->getCameraType();
    }



    this->_render(points_3d_raw, points_3d, cur_cam_ex_mat, cur_cam_in_mat, camera_type);
}

void mSceneUtils::_render(std::vector<glm::vec3> points_3d_raw, std::vector<glm::vec3> points_3d, glm::mat4 cur_cam_ex_mat, glm::mat4 cur_cam_in_mat, int camera_type) {

    for (int light_num = 0; light_num < mRenderParams::mLightSum; ++light_num) {
        this->VAO->bind();
        this->core_func->glViewport(0, 0, mRenderParams::mShadowWndWidth, mRenderParams::mShadowWndHeight);
        this->depth_shader->use();

        this->core_func->glBindFramebuffer(GL_FRAMEBUFFER, this->shadow_fbo[light_num]);
        this->core_func->glClear(GL_DEPTH_BUFFER_BIT);

        this->_setDepthShaderUniforms(light_num);

        if (this->is_with_floor) {
            this->_drawFloor();
        }
        if (points_3d.size() == this->pose_model->num_of_joints) {
            this->pose_model->draw(points_3d_raw, points_3d, cur_cam_ex_mat, cur_cam_in_mat, camera_type, 1);
        }
    }

    /************************** Render the scene **************************/
    this->VAO->bind();
    this->core_func->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    this->core_func->glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    this->core_func->glViewport(0, 0, this->wnd_width, this->wnd_height);

    this->scene_shader->use();
    this->_setSceneShaderUnoforms(glm::mat4(1.f), cur_cam_ex_mat, cur_cam_in_mat, mRenderParams::mShadowUseShadow);

    // Render the floor
    if (this->is_with_floor) {
        this->_drawFloor();
    }

    if (points_3d.size() == this->pose_model->num_of_joints) {
        this->pose_model->draw(points_3d_raw, points_3d, cur_cam_ex_mat, cur_cam_in_mat, camera_type, 0);
    }
}

/********** PAY ATTENTION: This function must be called after the person_center_pos is updated!!!!!!!!!! ************/
void mSceneUtils::renderCamerasPos(std::vector<const mCamera *> cameras) {
    this->VAO->bind();
    this->core_func->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    this->core_func->glViewport(0, 0, this->wnd_width, this->wnd_height);
    glm::mat4 cur_view_r_mat, cur_view_t_mat, cur_view_mat, cur_proj_mat;
    glm::vec3 camera_pos, visualize_camera_pos;

    cur_proj_mat = this->cur_camera->getProjMat();
    cur_view_mat = this->cur_camera->getViewMat(this->person_center_pos, &cur_view_r_mat, &cur_view_t_mat);

    for (int i = 0; i < cameras.size(); ++i) {
        this->scene_shader->use();
        glm::mat4 cur_model_mat = cameras[i]->getAffineMatrix(this->person_center_pos);
        this->_setSceneShaderUnoforms(cur_model_mat, cur_view_mat, cur_proj_mat, false);
        this->scene_shader->setVal("renderType", 0);
        this->scene_shader->setVal("fragColor", glm::vec3(1.0, 0.9294117647058824, 0.0745098039215686));

        this->camera_mesh->render(0);
    }
}
