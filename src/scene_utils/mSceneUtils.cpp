#include "mSceneUtils.h"


#include <iostream>
#include <glm/gtx/component_wise.hpp>
#include <QDebug>
#include "mRenderParameters.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include "mPoseDefs.h"

static float move_step = 80.0;

mSceneUtils::mSceneUtils(QOpenGLVertexArrayObject * vao, QOpenGLFunctions_3_3_Core * core_func, int wnd_width, int wnd_height, glm::mat4 cam_in_mat, glm::mat4 cam_ex_mat, bool is_ar, int pose_type) {
    this->wnd_width = wnd_width;
    this->wnd_height = wnd_height;
    this->is_ar = is_ar;
    this->VAO = vao;
    this->core_func = core_func;
    this->is_follow_person = false;
    this->is_focus_on_center = false;
    this->is_with_floor = true;
    this->person_center_pos = glm::vec3(0.f);
    this->cur_follow_dert = glm::vec3(0.f);
    this->prev_mouse_pos = glm::vec2(-1.f);

    // The parameter maybe changed as reality make sure the ground_col and ground_row is even
    this->ground_col = 100;
    this->ground_row = 100;

    this->setExMat(cam_ex_mat);
    this->setInMat(cam_in_mat);

    float target_model_size;
    if (is_ar) {
        target_model_size = 30 * 2;
        this->ground_size = 2000.0f;
        this->move_step_scale = 1.f;
        this->m_move_dir[0] = 1;this->m_move_dir[1] = -1;this->m_move_dir[2] = 1;
        this->m_rotate_dir[0] = 1;this->m_rotate_dir[1] = -1;
    }
    else {
        target_model_size = 2 * 1;
        this->ground_size = 20.f;
        this->move_step_scale = 0.03f;
        this->m_move_dir[0] = 1;this->m_move_dir[1] = 1;this->m_move_dir[2] = 1;
        this->m_rotate_dir[0] = 1; this->m_rotate_dir[1] = 1;
    }

    this->scene_shader = new mShader(mPoseShaderFiles[0], mPoseShaderFiles[1]);
    this->depth_shader = new mShader(mDepthShaderFiles[0], mDepthShaderFiles[1], mDepthShaderFiles[2]);

    this->pose_model = new mPoseModel(this->VAO, this->core_func, this->scene_shader, this->depth_shader, this->cam_proj_mat, target_model_size, is_ar, pose_type);

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
    this->shadow_fbo = std::vector<GLuint>(mLightSum, 0);
    this->shadow_tbo = std::vector<GLuint>(mLightSum, 0);

    this->core_func->glEnable(GL_DEPTH_TEST);
    this->core_func->glGenFramebuffers(mLightSum, &this->shadow_fbo[0]);
    this->core_func->glGenTextures(mLightSum, &this->shadow_tbo[0]);

    for (int light_num = 0; light_num < mLightSum; ++light_num) {
        this->core_func->glBindTexture(GL_TEXTURE_CUBE_MAP, this->shadow_tbo[light_num]);
        for (int i = 0; i < 6; ++i) {
            this->core_func->glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, mShadowWndWidth, mShadowWndHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
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
mSceneUtils::~mSceneUtils() {
    this->pose_model->~mPoseModel();
    this->scene_shader->~mShader();
    this->depth_shader->~mShader();
}
void mSceneUtils::setCurExMat(glm::mat4 cur_ex_mat) {
    this->cur_cam_ex_r_mat = glm::mat4(glm::mat3(cur_ex_mat));
    this->cur_cam_ex_t_mat = glm::inverse(this->cur_cam_ex_r_mat) * cur_ex_mat;
    this->cur_cam_ex_t_mat[0][1] = 0;this->cur_cam_ex_t_mat[0][2] = 0;this->cur_cam_ex_t_mat[1][0] = 0;this->cur_cam_ex_t_mat[1][2] = 0;this->cur_cam_ex_t_mat[2][0]= 0;this->cur_cam_ex_t_mat[2][1] = 0;
}
void mSceneUtils::setExMat(glm::mat4 & cam_ex_mat) {

    this->cam_ex_mat = glm::transpose(cam_ex_mat);
    this->cam_ex_mat_inverse = glm::inverse(this->cam_ex_mat);

    this->cam_ex_r_mat = glm::mat4(glm::mat3(this->cam_ex_mat));
    this->cam_ex_t_mat = glm::inverse(this->cam_ex_r_mat) * this->cam_ex_mat;
    this->cam_ex_t_mat[0][1] = 0;this->cam_ex_t_mat[0][2] = 0;this->cam_ex_t_mat[1][0] = 0;this->cam_ex_t_mat[1][2] = 0;this->cam_ex_t_mat[2][0]= 0;this->cam_ex_t_mat[2][1] = 0;

    this->cur_cam_ex_r_mat = this->cam_ex_r_mat;
    this->cur_cam_ex_t_mat = this->cam_ex_t_mat;

    glm::vec3 dir_z(-this->cur_cam_ex_r_mat[0][2], -this->cur_cam_ex_r_mat[1][2], -this->cur_cam_ex_r_mat[2][2]);
    glm::vec3 dir_x(this->cur_cam_ex_r_mat[0][0], this->cur_cam_ex_r_mat[1][0], this->cur_cam_ex_r_mat[2][0]);
    glm::vec3 dir_y(this->cur_cam_ex_r_mat[0][1], this->cur_cam_ex_r_mat[1][1], this->cur_cam_ex_r_mat[2][1]);

    qDebug() << dir_x.x << " " << dir_x.y << " " << dir_x.z;
    qDebug() << dir_y.x << " " << dir_y.y << " " << dir_y.z;
    qDebug() << dir_z.x << " " << dir_z.y << " " << dir_z.z;
    qDebug() << glm::dot(dir_z, dir_y) << " " << glm::dot(dir_z, dir_x) << " " << glm::dot(dir_x, dir_y);
}

void mSceneUtils::setInMat(glm::mat4 & cam_in_mat) {
    this->cam_in_mat = cam_in_mat;
    if (this->is_ar) {
        this->cam_proj_mat = glm::transpose(glm::mat4({
            2.0*this->cam_in_mat[0][0] / wnd_width, 0, -1 + 2.0*this->cam_in_mat[0][2] / wnd_width, 0.0,
            0, -2.0*this->cam_in_mat[1][1]/wnd_height, 1 - 2.0*this->cam_in_mat[1][2] / wnd_height, 0.0,
            0, 0, 1, -2 * this->cam_in_mat[0][0],
            0, 0, 1, 0}));
    }
    else {
        this->cam_proj_mat = glm::transpose(this->cam_in_mat);
    }
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
    if (std::abs(move_type) == 1 && event->buttons() & Qt::MiddleButton) {
        // set the prev_mouse_pos
        this->prev_mouse_pos = glm::vec2(event->pos().x(), event->pos().y());
    }
    else if (std::abs(move_type) == 2 && event->buttons() & Qt::MiddleButton) {
        glm::vec3 dir_x(this->cur_cam_ex_r_mat[0][0], this->cur_cam_ex_r_mat[1][0], this->cur_cam_ex_r_mat[2][0]);
        glm::vec3 dir_y(this->cur_cam_ex_r_mat[0][1], this->cur_cam_ex_r_mat[1][1], this->cur_cam_ex_r_mat[2][1]);

        glm::vec2 cur_mouse_pos = glm::vec2(event->pos().x(), event->pos().y());
        glm::vec2 move_dert = (cur_mouse_pos - this->prev_mouse_pos);

        this->cur_cam_ex_t_mat = glm::translate(glm::translate(this->cur_cam_ex_t_mat, -move_dert.x * this->move_step_scale * 33.3333f * dir_x * this->m_move_dir[0]), move_dert.y * this->move_step_scale * 33.3333f * dir_y * m_move_dir[1]);

        this->prev_mouse_pos = cur_mouse_pos;
    }
    else if (std::abs(move_type) == 3) {
        glm::vec3 dir_z(-this->cur_cam_ex_r_mat[0][2], -this->cur_cam_ex_r_mat[1][2], -this->cur_cam_ex_r_mat[2][2]);
        glm::vec3 dir_x(this->cur_cam_ex_r_mat[0][0], this->cur_cam_ex_r_mat[1][0], this->cur_cam_ex_r_mat[2][0]);
        glm::vec3 dir_y(this->cur_cam_ex_r_mat[0][1], this->cur_cam_ex_r_mat[1][1], this->cur_cam_ex_r_mat[2][1]);
//        glm::vec3 dir_z = glm::cross(dir_x, dir_y);
        qDebug() << dir_x.x << " " << dir_x.y << " " << dir_x.z;
        qDebug() << dir_y.x << " " << dir_y.y << " " << dir_y.z;
        qDebug() << dir_z.x << " " << dir_z.y << " " << dir_z.z;
        qDebug() << glm::dot(dir_z, dir_y) << " " << glm::dot(dir_z, dir_x) << " " << glm::dot(dir_x, dir_y);
        qDebug() << glm::dot(dir_z, dir_z) << " " << glm::dot(dir_y, dir_y) << " " << glm::dot(dir_x, dir_x);
        float sign = move_type > 0?1.f:-1.f;
        this->cur_cam_ex_t_mat = glm::translate(this->cur_cam_ex_t_mat, sign * move_step * dir_z * this->move_step_scale * 4.f * this->m_move_dir[2]);
    }
    else if (std::abs(move_type) == 0 && this->is_follow_person) {
        // the cam_ex_t_mat is -(camera pos)
        if (this->cur_follow_dert == glm::vec3(0.f)) {
            // set the default pose
            this->setFollowDefault();
        }
        else {
            this->cur_follow_dert = glm::vec3(-this->cur_cam_ex_t_mat[3][0] - this->person_center_pos[0], -this->cur_cam_ex_t_mat[3][1] - this->person_center_pos[1], -this->cur_cam_ex_t_mat[3][2] - this->person_center_pos[2]);
        }
    }
}
void mSceneUtils::rotateCamrea(const glm::mat4 &rotate_mat) {
    this->cur_cam_ex_r_mat = this->cur_cam_ex_r_mat * rotate_mat;
}
void mSceneUtils::getCurExMat(glm::mat4 & cam_ex_r_mat, glm::mat4 & cam_ex_t_mat) {
    if (this->is_follow_person) {
        // the cam_ex_t_mat is -(camera pos)
        // only update the cur_follow_dert
        this->cur_cam_ex_t_mat[3][0] = -(this->cur_follow_dert[0] + this->person_center_pos[0]);
        this->cur_cam_ex_t_mat[3][1] = -(this->cur_follow_dert[1] + this->person_center_pos[1]);
        this->cur_cam_ex_t_mat[3][2] = -(this->cur_follow_dert[2] + this->person_center_pos[2]);
    }

    if (this->is_focus_on_center) {
        glm::vec3 camera_pos(-this->cur_cam_ex_t_mat[3][0], -this->cur_cam_ex_t_mat[3][1], -this->cur_cam_ex_t_mat[3][2]);
        glm::vec3 z_axis = glm::normalize(camera_pos - this->person_center_pos);
        glm::vec3 x_axis = glm::normalize(glm::cross(glm::vec3(0, 1, 0), z_axis));
        glm::vec3 y_axis = glm::normalize(glm::cross(z_axis, x_axis));
        this->cur_cam_ex_r_mat[0][0] = x_axis.x;this->cur_cam_ex_r_mat[0][1] = y_axis.x;this->cur_cam_ex_r_mat[0][2] = z_axis.x;
        this->cur_cam_ex_r_mat[1][0] = x_axis.y;this->cur_cam_ex_r_mat[1][1] = y_axis.y;this->cur_cam_ex_r_mat[1][2] = z_axis.y;
        this->cur_cam_ex_r_mat[2][0] = x_axis.z;this->cur_cam_ex_r_mat[2][1] = y_axis.z;this->cur_cam_ex_r_mat[2][2] = z_axis.z;
    }
    cam_ex_r_mat = this->cur_cam_ex_r_mat;
    cam_ex_t_mat = this->cur_cam_ex_t_mat;
}
glm::mat4 mSceneUtils::getCurExMat() {
    glm::mat4 cur_r_mat, cur_t_mat;
    this->getCurExMat(cur_r_mat, cur_t_mat);
    return cur_r_mat * cur_t_mat;
}
glm::mat4 mSceneUtils::getRawExMat() {
    return this->cam_ex_mat;
}

glm::vec3 mSceneUtils::getCurFollowVec() {
    return this->cur_follow_dert;
}
void mSceneUtils::setCurFollowVec(glm::vec3 cur_follow_vec) {
    this->cur_follow_dert = cur_follow_vec;
}
void mSceneUtils::setFollowPerson(bool is_follow) {
    this->is_follow_person = is_follow;
    if (is_follow) {
        this->moveCamera(0);
    }
}
void mSceneUtils::setFocusOnCenter(bool is_focus_on_center) {
    this->is_focus_on_center = is_focus_on_center;
}
void mSceneUtils::captureFrame(cv::Mat & cur_frame) {
    this->core_func->glReadBuffer(GL_FRONT);
    if (cur_frame.size().height != this->wnd_height || cur_frame.size().width != this->wnd_width) {
        cur_frame = cv::Mat(this->wnd_height, this->wnd_width, CV_8UC3);
    }
    this->core_func->glReadPixels(0, 0, this->wnd_width, this->wnd_height, GL_BGR, GL_UNSIGNED_BYTE, cur_frame.ptr<unsigned char>());
    cv::flip(cur_frame, cur_frame, 0);
}
void mSceneUtils::setFloor(bool is_with_floor) {
    this->is_with_floor = is_with_floor;
}

void mSceneUtils::getSplittedCameras(int camera_num, std::vector<glm::vec3> &splitted_cameras) {
    splitted_cameras.clear();
    glm::vec3 cur_cam_pos(-this->cur_cam_ex_t_mat[3][0], -this->cur_cam_ex_t_mat[3][1], -this->cur_cam_ex_t_mat[3][2]);
    glm::vec3 splitted_center(this->person_center_pos.x, cur_cam_pos.y, this->person_center_pos.z);

    float per_angle = 2*3.1415927 / camera_num;
    glm::vec3 cur_vec = glm::normalize(cur_cam_pos - splitted_center);
    float cur_r = glm::length(splitted_center - cur_cam_pos);
    glm::mat4 rotate_mat = glm::rotate(glm::mat4(1.f), per_angle, glm::vec3(0, 1, 0));
    for (int i = 0; i < camera_num; ++i) {
        cur_vec = glm::vec3(rotate_mat * glm::vec4(cur_vec, 1.0));
        splitted_cameras.push_back(cur_vec * cur_r);
    }
}
void mSceneUtils::setFollowDefault() {
    this->cur_follow_dert = glm::vec3(0.f, 0.f, mPoseDef::bones_length[4] * 6);
}

glm::mat4 mSceneUtils::convertVec2Mat(const glm::vec3 & follow_vec, glm::vec3 person_center_pos) {

    glm::mat4 cur_view_t_mat(1.f);
    glm::mat4 cur_view_r_mat(1.f);

    cur_view_t_mat[3][0] = -(follow_vec[0] + person_center_pos[0]);
    cur_view_t_mat[3][1] = -(follow_vec[1] + person_center_pos[1]);
    cur_view_t_mat[3][2] = -(follow_vec[2] + person_center_pos[2]);

    glm::vec3 camera_pos(-cur_view_t_mat[3][0], -cur_view_t_mat[3][1], -cur_view_t_mat[3][2]);

    glm::vec3 z_axis = glm::normalize(camera_pos - person_center_pos);
    glm::vec3 x_axis = glm::normalize(glm::cross(glm::vec3(0, 1, 0), z_axis));
    glm::vec3 y_axis = glm::normalize(glm::cross(z_axis, x_axis));

    cur_view_r_mat[0][0] = x_axis.x;cur_view_r_mat[0][1] = y_axis.x;cur_view_r_mat[0][2] = z_axis.x;
    cur_view_r_mat[1][0] = x_axis.y;cur_view_r_mat[1][1] = y_axis.y;cur_view_r_mat[1][2] = z_axis.y;
    cur_view_r_mat[2][0] = x_axis.z;cur_view_r_mat[2][1] = y_axis.z;cur_view_r_mat[2][2] = z_axis.z;

    return cur_view_r_mat * cur_view_t_mat;
}

// TODO: The 3D view coordinate labels need to be checked.
//       THe 2D is right, after visualized.
void mSceneUtils::_getLabelsFromFrame(const std::vector<glm::vec3> & joints, const glm::mat4 & view_mat, std::vector<glm::vec2> & labels_2d, std::vector<glm::vec3> & labels_3d) {
    if (joints.size() != 0) {
        labels_2d = std::vector<glm::vec2>(joints.size());
        labels_3d = std::vector<glm::vec3>(joints.size());
        glm::mat4 cur_view_mat = view_mat;
        glm::vec3 root_joint = glm::vec3(cur_view_mat * glm::vec4(joints[joints.size() - 1], 1.f));

        for (int i = 0; i < joints.size(); ++i) {
            glm::vec4 cur_2d = this->cam_proj_mat * cur_view_mat * glm::vec4(joints[i], 1.0);
            cur_2d /= cur_2d.w;
            labels_2d[i] = glm::vec2(this->wnd_width * (cur_2d.x + 1.f) / 2.f , this->wnd_height * (1.f - cur_2d.y) / 2.f);
            labels_3d[i] = glm::vec3(cur_view_mat * glm::vec4(joints[i], 1.f)) - root_joint;

            // Cause the camera in the real word
            labels_3d[i].y *= -1;
            labels_3d[i].z *= -1;
        }
        mPoseDef::scalePose(labels_3d);
    }
}

void mSceneUtils::getLabelsFromFrame(const std::vector<glm::vec3> & joints, const glm::mat4 & view_mat, std::vector<glm::vec2> & labels_2d, std::vector<glm::vec3> & labels_3d) {
    // Joints is in the global coordinate
    this->_getLabelsFromFrame(joints, view_mat, labels_2d, labels_3d);
}

void mSceneUtils::getLabelsFromFrame(const std::vector<glm::vec3> & joints, const glm::vec3 & view_vec, std::vector<glm::vec2> & labels_2d, std::vector<glm::vec3> & labels_3d) {
    // Joints is in the global coordinate
    glm::vec3 root_joint = joints[joints.size() - 1];
    glm::mat4 cur_view_mat = this->convertVec2Mat(view_vec, root_joint);
    this->_getLabelsFromFrame(joints, cur_view_mat, labels_2d, labels_3d);
}

void mSceneUtils::_beforeRender(const std::vector<glm::vec3> & points_3d) {
    // points_3d is in the global coordinate
    if (points_3d.size() == this->pose_model->num_of_joints) {
        this->person_center_pos = points_3d[points_3d.size()-1];
    }

//    // correct the x direction
//    glm::vec3 dir_z(this->cur_cam_ex_r_mat[0][2], this->cur_cam_ex_r_mat[1][2], this->cur_cam_ex_r_mat[2][2]);
//    // The default head position
//    glm::vec3 dir_y(0, 1, 0);
//    glm::vec3 dir_x = glm::normalize(glm::cross(dir_y, dir_z));
//    this->cur_cam_ex_r_mat[0][0] = dir_x.x;
//    this->cur_cam_ex_r_mat[1][0] = dir_x.y;
//    this->cur_cam_ex_r_mat[2][0] = dir_x.z;
}

void mSceneUtils::render(std::vector<glm::vec3> points_3d, glm::mat4 cam_ex_mat) {
    this->_beforeRender(points_3d);

    glm::mat4 cur_cam_ex_mat;
    if (cam_ex_mat == glm::mat4(0.f)) {
        cur_cam_ex_mat = this->getCurExMat();
    }
    else {
        cur_cam_ex_mat = cam_ex_mat;
    }

    this->_render(points_3d, cur_cam_ex_mat);
}

void mSceneUtils::render(std::vector<glm::vec3> points_3d, glm::vec3 cam_ex_vec) {
    this->_beforeRender(points_3d);

    glm::mat4 cur_cam_ex_mat;
    if (cam_ex_vec == glm::vec3(0.f)) {
        cur_cam_ex_mat = this->convertVec2Mat(this->getCurFollowVec(), this->person_center_pos);
    }
    else {
        cur_cam_ex_mat = this->convertVec2Mat(cam_ex_vec, this->person_center_pos);
    }

    this->_render(points_3d, cur_cam_ex_mat);
}

void mSceneUtils::_render(std::vector<glm::vec3> points_3d, glm::mat4 cur_cam_ex_mat) {

    for (int light_num = 0; light_num < mLightSum; ++light_num) {
        this->VAO->bind();
        this->core_func->glViewport(0, 0, mShadowWndWidth, mShadowWndHeight);
        this->depth_shader->use();

        this->core_func->glBindFramebuffer(GL_FRAMEBUFFER, this->shadow_fbo[light_num]);
        this->core_func->glClear(GL_DEPTH_BUFFER_BIT);

        if (this->is_ar) {
            for (int i = 0; i < 6; ++i) {
                this->depth_shader->setVal(("shadow_mat["+std::to_string(i)+"]").c_str(), mShadowTransforms_AR[light_num][i]);
            }
            this->depth_shader->setVal("far_plane", mShadowFarPlane_AR);
            this->depth_shader->setVal("lightPos", mLightPos_AR[light_num]);
        }
        else {
            for (int i = 0; i < 6; ++i) {
                this->depth_shader->setVal(("shadow_mat["+std::to_string(i)+"]").c_str(), mShadowTransforms[light_num][i]);
            }
            this->depth_shader->setVal("far_plane", mShadowFarPlane);
            this->depth_shader->setVal("lightPos", mLightPos[light_num]);
        }
        this->depth_shader->setVal("model", glm::mat4(1.f));

        if (this->is_with_floor) {
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

        if (points_3d.size() == this->pose_model->num_of_joints) {
            this->pose_model->draw(points_3d, cur_cam_ex_mat, 1);
        }
    }

    /************************** Render the scene **************************/
    this->VAO->bind();
    this->core_func->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    this->core_func->glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    this->core_func->glViewport(0, 0, this->wnd_width, this->wnd_height);

    this->scene_shader->use();
    glm::mat4 view_r_mat = glm::mat4(glm::mat3(cur_cam_ex_mat));
    glm::mat4 view_t_mat = glm::inverse(view_r_mat) * cur_cam_ex_mat;
    this->scene_shader->setVal("renderType", 1);
    this->scene_shader->setVal("use_shadow", mShadowUseShadow);
    this->scene_shader->setVal("viewPos", glm::vec3(view_t_mat[3][0], view_t_mat[3][1], view_t_mat[3][2]));
    this->scene_shader->setVal("projection", this->cam_proj_mat);
    this->scene_shader->setVal("view", cur_cam_ex_mat);
    this->scene_shader->setVal("model", glm::mat4(1.f));
    this->scene_shader->setVal("normMat", glm::transpose(glm::inverse(glm::mat4(1.f))));

    if (this->is_ar) {
        this->scene_shader->setVal("far_plane", mShadowFarPlane_AR);
        this->scene_shader->setVal("shadow_bias", mBias_AR);
    }
    else {
        this->scene_shader->setVal("far_plane", mShadowFarPlane);
        this->scene_shader->setVal("shadow_bias", mBias);
    }

    for (int light_num = 0; light_num < mLightSum; ++light_num) {
        this->scene_shader->setVal(("pointLights[" + std::to_string(light_num) + "].ambient").c_str(), mAmbient);
        this->scene_shader->setVal(("pointLights[" + std::to_string(light_num) + "].diffuse").c_str(), mDiffuse);
        this->scene_shader->setVal(("pointLights[" + std::to_string(light_num) + "].specular").c_str(), mSpecular);
        if (this->is_ar) {
            this->scene_shader->setVal(("pointLights[" + std::to_string(light_num) + "].position").c_str(), mLightPos_AR[light_num]);
        }
        else {
            this->scene_shader->setVal(("pointLights[" + std::to_string(light_num) + "].position").c_str(), mLightPos[light_num]);
        }

        this->scene_shader->setVal(("depth_cube["+ std::to_string(light_num) + "]").c_str(), 1 + light_num);

        this->core_func->glActiveTexture(GL_TEXTURE1 + light_num);
        this->core_func->glBindTexture(GL_TEXTURE_CUBE_MAP, this->shadow_fbo[light_num]);
    }

    if (this->is_with_floor) {
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

    if (points_3d.size() == this->pose_model->num_of_joints) {
        this->pose_model->draw(points_3d, cur_cam_ex_mat, 0);
    }
}
