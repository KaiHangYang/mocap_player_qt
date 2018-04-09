#include "mSceneUtils.h"

#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <glm/gtx/component_wise.hpp>
#include <QDebug>
#include "mRenderParameters.h"

static float move_step = 80.0;

static bool is_surround = false;
static float surround_step = 1.0f;
static bool is_surround_still = false;

mSceneUtils::mSceneUtils(QOpenGLVertexArrayObject * vao, QOpenGLFunctions_3_3_Core * core_func, int wnd_width, int wnd_height, glm::mat4 cam_in_mat, glm::mat4 cam_ex_mat, bool is_ar, int pose_type) {
    this->wnd_width = wnd_width;
    this->wnd_height = wnd_height;
    this->is_ar = is_ar;
    this->VAO = vao;
    this->core_func = core_func;

    // The parameter maybe changed as reality make sure the ground_col and ground_row is even
    this->ground_col = 200;
    this->ground_row = 200;

    this->do_use_surround = false;
    this->surround_center = glm::vec3(0.f, 0.f, 0.f);

    this->setExMat(cam_ex_mat);
    this->setInMat(cam_in_mat);

    float target_model_size;
    if (is_ar) {
        target_model_size = 30 * 2;
        this->ground_size = 2000.0f;
        this->move_step_scale = 1.f;

    }
    else {
        target_model_size = 2 * 1;
        this->ground_size = 20.f;
        this->move_step_scale = 0.03f;
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
    this->core_func->glEnable(GL_DEPTH_TEST);
    this->core_func->glGenFramebuffers(1, &this->shadow_fbo);
    this->core_func->glGenTextures(1, &this->shadow_tbo);

    this->core_func->glBindTexture(GL_TEXTURE_CUBE_MAP, this->shadow_tbo);
    for (int i = 0; i < 6; ++i) {
        this->core_func->glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, mShadowWndWidth, mShadowWndHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }

    this->core_func->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    this->core_func->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    this->core_func->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    this->core_func->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    this->core_func->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    this->core_func->glBindFramebuffer(GL_FRAMEBUFFER, this->shadow_fbo);
    this->core_func->glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->shadow_tbo, 0);
    // Tell OpenGL not to render color buffer
    this->core_func->glDrawBuffer(GL_NONE);
    this->core_func->glReadBuffer(GL_NONE);

    if (this->core_func->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        qDebug() << "(mSceneUtils.cpp) Shadow Framebuffer is not complete!";
        exit(-1);
    }
    this->core_func->glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /*******************************************************************************/
    this->VAO->release();
}
void mSceneUtils::setExMat(glm::mat4 & cam_ex_mat) {

    this->cam_ex_mat = glm::transpose(cam_ex_mat);
    this->cam_ex_mat_inverse = glm::inverse(this->cam_ex_mat);

    this->cam_ex_r_mat = glm::mat4(glm::mat3(this->cam_ex_mat));
    this->cam_ex_t_mat = glm::inverse(this->cam_ex_r_mat) * this->cam_ex_mat;
    this->cam_ex_t_mat[0][1] = 0;this->cam_ex_t_mat[0][2] = 0;this->cam_ex_t_mat[1][0] = 0;this->cam_ex_t_mat[1][2] = 0;this->cam_ex_t_mat[2][0]= 0;this->cam_ex_t_mat[2][1] = 0;

    this->cur_cam_ex_r_mat = this->cam_ex_r_mat;
    this->cur_cam_ex_t_mat = this->cam_ex_t_mat;
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
mSceneUtils::~mSceneUtils() {
    this->pose_model->~mPoseModel();
    this->scene_shader->~mShader();
    this->depth_shader->~mShader();
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

void mSceneUtils::moveCamera(int move_dir) {

    if (move_dir != 0) {
        glm::vec3 dir_x(this->cur_cam_ex_r_mat[0][0], this->cur_cam_ex_r_mat[1][0], this->cur_cam_ex_r_mat[2][0]);
        glm::vec3 dir_y(this->cur_cam_ex_r_mat[0][1], this->cur_cam_ex_r_mat[1][1], this->cur_cam_ex_r_mat[2][1]);
        glm::vec3 dir_z(-this->cur_cam_ex_r_mat[0][2], -this->cur_cam_ex_r_mat[1][2], -this->cur_cam_ex_r_mat[2][2]);

        if (move_dir == 1) {
            this->cur_cam_ex_t_mat = glm::translate(this->cur_cam_ex_t_mat, -move_step * dir_x * this->move_step_scale);
        }
        else if (move_dir == -1) {
            this->cur_cam_ex_t_mat = glm::translate(this->cur_cam_ex_t_mat, move_step * dir_x * this->move_step_scale);
        }
        else if (move_dir == 2) {
            this->cur_cam_ex_t_mat = glm::translate(this->cur_cam_ex_t_mat, -move_step * dir_y * this->move_step_scale / 2.f);
        }
        else if (move_dir == -2) {
            this->cur_cam_ex_t_mat = glm::translate(this->cur_cam_ex_t_mat, move_step * dir_y * this->move_step_scale / 2.f);
        }
        else if (move_dir == -3) {
            this->cur_cam_ex_t_mat = glm::translate(this->cur_cam_ex_t_mat, -move_step * dir_z * this->move_step_scale * 3.f);
        }
        else if (move_dir == 3) {
            this->cur_cam_ex_t_mat = glm::translate(this->cur_cam_ex_t_mat, move_step * dir_z * this->move_step_scale * 3.f);
        }
        move_dir = 0;
    }
}
void mSceneUtils::rotateCamrea(const glm::mat4 &rotate_mat) {
    this->cur_cam_ex_r_mat = this->cur_cam_ex_r_mat * rotate_mat;
}
void mSceneUtils::getCurExMat(glm::mat4 & cam_ex_r_mat, glm::mat4 & cam_ex_t_mat) {
    cam_ex_r_mat = this->cur_cam_ex_r_mat;
    cam_ex_t_mat = this->cur_cam_ex_t_mat;
}
/********************* Going to set the rotate around one person ********************/
void mSceneUtils::surroundOnePoint(glm::mat4 & model_mat) {
    //std::cout << this->surround_center[0] << this->surround_center[1] << this->surround_center[2] << std::endl;
    model_mat = glm::translate(glm::mat4(1.f), this->surround_center) * glm::rotate(glm::mat4(1.f), glm::radians(surround_step), glm::vec3(0, 1, 0)) * glm::translate(glm::mat4(1.f), -this->surround_center);
}

void mSceneUtils::setSurround(bool do_surround, glm::vec3 surround_center) {
    this->surround_center = surround_center;
    this->do_use_surround = do_surround;
}

void mSceneUtils::render(std::vector<float> points_3d) {
    // correct the x direction
    glm::vec3 dir_z(this->cur_cam_ex_r_mat[0][2], this->cur_cam_ex_r_mat[1][2], this->cur_cam_ex_r_mat[2][2]);
    // The default head position
    glm::vec3 dir_y(0, 1, 0);
    glm::vec3 dir_x = glm::normalize(glm::cross(dir_y, dir_z));
    this->cur_cam_ex_r_mat[0][0] = dir_x.x;
    this->cur_cam_ex_r_mat[1][0] = dir_x.y;
    this->cur_cam_ex_r_mat[2][0] = dir_x.z;

    glm::mat4 cur_cam_ex_mat = this->cur_cam_ex_r_mat * this->cur_cam_ex_t_mat;

    this->VAO->bind();
    this->core_func->glViewport(0, 0, mShadowWndWidth, mShadowWndHeight);
    this->core_func->glBindFramebuffer(GL_FRAMEBUFFER, this->shadow_fbo);
    this->core_func->glClear(GL_DEPTH_BUFFER_BIT);
    this->depth_shader->use();

    if (this->is_ar) {
        for (int i = 0; i < 6; ++i) {
            this->depth_shader->setVal(("shadow_mat["+std::to_string(i)+"]").c_str(), mShadowTransforms_AR[i]);
        }
        this->depth_shader->setVal("far_plane", mShadowFarPlane_AR);
        this->depth_shader->setVal("lightPos", mLightPos_AR);
    }
    else {

        for (int i = 0; i < 6; ++i) {
            this->depth_shader->setVal(("shadow_mat["+std::to_string(i)+"]").c_str(), mShadowTransforms[i]);
        }
        this->depth_shader->setVal("far_plane", mShadowFarPlane);
        this->depth_shader->setVal("lightPos", mLightPos);
    }

    this->depth_shader->setVal("model", glm::mat4(1.f));

    this->core_func->glBindBuffer(GL_ARRAY_BUFFER, this->ground_vbo);
    this->core_func->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    this->core_func->glEnableVertexAttribArray(0);

    this->core_func->glBindBuffer(GL_ARRAY_BUFFER, this->ground_cbo);
    this->core_func->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    this->core_func->glEnableVertexAttribArray(1);

    this->core_func->glDrawArrays(GL_TRIANGLES, 0, this->array_size);

    this->core_func->glDisableVertexAttribArray(0);
    this->core_func->glDisableVertexAttribArray(1);

    if (points_3d.size() == 3*this->pose_model->num_of_joints) {
        this->pose_model->draw(points_3d, this->cam_ex_mat_inverse, cur_cam_ex_mat, 1);
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
    // Set the light parameter
    this->scene_shader->setVal("pointLights[0].ambient", mAmbient);
    this->scene_shader->setVal("pointLights[0].diffuse", mDiffuse);
    this->scene_shader->setVal("pointLights[0].specular", mSpecular);
    this->scene_shader->setVal("viewPos", glm::vec3(view_t_mat[3][0], view_t_mat[3][1], view_t_mat[3][2]));
    this->scene_shader->setVal("projection", this->cam_proj_mat);
    this->scene_shader->setVal("view", cur_cam_ex_mat);
    this->scene_shader->setVal("model", glm::mat4(1.f));
    this->scene_shader->setVal("normMat", glm::transpose(glm::inverse(glm::mat4(1.f))));

    this->scene_shader->setVal("depth_cube", 1);

    if (this->is_ar) {
        this->scene_shader->setVal("pointLights[0].position", mLightPos_AR);
        this->scene_shader->setVal("far_plane", mShadowFarPlane_AR);
        this->scene_shader->setVal("shadow_bias", mBias_AR);
    }
    else {
        this->scene_shader->setVal("pointLights[0].position", mLightPos);
        this->scene_shader->setVal("far_plane", mShadowFarPlane);
        this->scene_shader->setVal("shadow_bias", mBias);
    }

    this->core_func->glActiveTexture(GL_TEXTURE1);
    this->core_func->glBindTexture(GL_TEXTURE_CUBE_MAP, this->shadow_fbo);

    this->core_func->glBindBuffer(GL_ARRAY_BUFFER, this->ground_vbo);
    this->core_func->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    this->core_func->glEnableVertexAttribArray(0);

    this->core_func->glBindBuffer(GL_ARRAY_BUFFER, this->ground_cbo);
    this->core_func->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    this->core_func->glEnableVertexAttribArray(1);

    this->core_func->glDrawArrays(GL_TRIANGLES, 0, this->array_size);

    this->core_func->glDisableVertexAttribArray(0);
    this->core_func->glDisableVertexAttribArray(1);

    if (points_3d.size() == 3*this->pose_model->num_of_joints) {
        this->pose_model->draw(points_3d, this->cam_ex_mat_inverse, cur_cam_ex_mat, 0, 1);
    }
}

