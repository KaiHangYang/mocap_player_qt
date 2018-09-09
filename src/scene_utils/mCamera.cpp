#include "mCamera.h"
#include <glm/gtx/component_wise.hpp>
#include <glm/gtc/matrix_transform.hpp>

// suppose the inputed camera matrix is row major
mCamera::mCamera(const mCamera *camera) {
    *this = *camera;
}
mCamera::mCamera(glm::mat4 proj_mat, glm::mat4 view_mat, int camera_type, int wnd_width, int wnd_height, bool is_ar) {
    this->wnd_height = wnd_height;
    this->wnd_width = wnd_width;
    this->is_ar = is_ar;
    this->is_focus = false;

    this->setCameraType(camera_type);

    // set the intrinsic matrix of the camera
    this->setProjMat(proj_mat);
    view_mat = glm::transpose(view_mat);
    this->setViewMat(view_mat);
}

mCamera::~mCamera() {}
mCamera& mCamera::operator=(const mCamera &a) {
    this->wnd_height = a.wnd_height;
    this->wnd_width = a.wnd_width;
    this->is_ar = a.is_ar;
    this->is_focus = a.is_focus;
    this->is_follow = a.is_follow;
    this->proj_mat = a.proj_mat;
    this->view_r_mat = a.view_r_mat;
    this->view_t_mat = a.view_t_mat;
    this->follow_dert = a.follow_dert;
    this->camera_type = a.camera_type; // is the camera use perspective(0) or my 'ortho' proj_mat(1)
    return *this;
}

void mCamera::updateViewMat(glm::vec3 pose_center) {
    glm::mat4 cur_view_t_mat, cur_view_r_mat;
    this->getViewMat(pose_center, &cur_view_r_mat, &cur_view_t_mat);

    this->view_t_mat = cur_view_t_mat;
    this->view_r_mat = cur_view_r_mat;
}

void mCamera::updateViewVec(glm::vec3 pose_center) {
    // Here the view_t_mat is already setted.
    this->follow_dert = glm::vec3(-this->view_t_mat[3][0] - pose_center[0], -this->view_t_mat[3][1] - pose_center[1], -this->view_t_mat[3][2] - pose_center[2]);
}

void mCamera::setFocus(bool is_focus) {
    this->is_focus = is_focus;
}
void mCamera::setFollow(bool is_follow, glm::vec3 pose_center) {
    this->is_follow = is_follow;
    if (this->is_follow) {
        this->updateViewVec(pose_center);
    }
    else {
        this->updateViewMat(pose_center);
    }
}

void mCamera::setCameraType(int camera_type) {
    this->camera_type = camera_type;
}

glm::mat4 mCamera::getProjMat() const {
    return this->proj_mat;
}

glm::mat4 mCamera::getViewMat(glm::vec3 pose_center, glm::mat4 * out_view_r_mat, glm::mat4 * out_view_t_mat) const {
    glm::mat4 view_t_mat = this->view_t_mat;
    glm::mat4 view_r_mat = this->view_r_mat;
    if (this->is_follow) {
        view_t_mat[3][0] = -(this->follow_dert[0] + pose_center[0]);
        view_t_mat[3][1] = -(this->follow_dert[1] + pose_center[1]);
        view_t_mat[3][2] = -(this->follow_dert[2] + pose_center[2]);
    }
    if (this->is_focus) {
        glm::vec3 camera_pos(-view_t_mat[3][0], -view_t_mat[3][1], -view_t_mat[3][2]);
        glm::vec3 z_axis = glm::normalize(camera_pos - pose_center);
        glm::vec3 x_axis = glm::normalize(glm::cross(glm::vec3(0, 1, 0), z_axis));
        glm::vec3 y_axis = glm::normalize(glm::cross(z_axis, x_axis));
        view_r_mat[0][0] = x_axis.x;view_r_mat[0][1] = y_axis.x;view_r_mat[0][2] = z_axis.x;
        view_r_mat[1][0] = x_axis.y;view_r_mat[1][1] = y_axis.y;view_r_mat[1][2] = z_axis.y;
        view_r_mat[2][0] = x_axis.z;view_r_mat[2][1] = y_axis.z;view_r_mat[2][2] = z_axis.z;
    }
    if (out_view_r_mat) {
        *out_view_r_mat = view_r_mat;
    }
    if (out_view_t_mat) {
        *out_view_t_mat = view_t_mat;
    }

    return view_r_mat * view_t_mat;
}
glm::vec3 mCamera::getViewVec() const {
    return this->follow_dert;
}

void mCamera::setViewVec(glm::vec3 view_vec) {
    this->follow_dert = view_vec;
}

void mCamera::setViewMat(glm::mat4 view_mat) {
    // Then set the extrinsic matrix of the camera.
    // Here I get r and t matrix from the view matrix
    this->view_r_mat = glm::mat4(glm::mat3(view_mat));
    this->view_t_mat = glm::inverse(this->view_r_mat) * view_mat;
    this->view_t_mat[0][1] = this->view_t_mat[0][2] = this->view_t_mat[1][0] = this->view_t_mat[1][2] = this->view_t_mat[2][0]= this->view_t_mat[2][1] = 0;
}

void mCamera::setProjMat(glm::mat4 proj_mat) {
    if (this->is_ar) {
        // Currently, the below code works, Now I will make sure from the procedure of calculation
        // The new z projected is only used for clip, so I can assume the near plane 1 and far plane inf. without change the new image(x and y);
        // See http://www.songho.ca/opengl/gl_projectionmatrix.html
        this->proj_mat = glm::transpose(glm::mat4({
            2.0*proj_mat[0][0] / this->wnd_width, 0, -1 + 2.0*proj_mat[0][2] / this->wnd_width, 0.0,
            0, -2.0*proj_mat[1][1]/this->wnd_height, 1 - 2.0*proj_mat[1][2] / this->wnd_height, 0.0,
            0, 0, 1, -2 * 1,
            0, 0, 1, 0}));
    }
    else {
        this->proj_mat = glm::transpose(proj_mat);
    }
}

void mCamera::getSplittedCameras(int camera_num, glm::vec3 pose_center, std::vector<glm::mat4> &splitted_cameras) {
    this->updateViewMat(pose_center);
    splitted_cameras.clear();

    glm::mat4 tmp_ex_r_mat(this->view_r_mat);
    glm::mat4 tmp_ex_t_mat(1.0);

    glm::vec3 cur_cam_pos(-this->view_t_mat[3][0], -this->view_t_mat[3][1], -this->view_t_mat[3][2]);
    glm::vec3 splitted_center(pose_center.x, cur_cam_pos.y, pose_center.z);
    float per_angle = 2 * 3.1415927 / camera_num;
    glm::vec3 cur_vec = glm::normalize(cur_cam_pos - splitted_center);
    float cur_r = glm::length(splitted_center - cur_cam_pos);

    glm::mat4 rotate_mat = glm::rotate(glm::mat4(1.f), per_angle, glm::vec3(0, 1, 0));
    for (int i = 0; i < camera_num; ++i) {
        cur_vec = glm::vec3(rotate_mat * glm::vec4(cur_vec, 1.f)); // New camera position
        glm::vec4 cur_axis_x(tmp_ex_r_mat[0][0], tmp_ex_r_mat[1][0], tmp_ex_r_mat[2][0], 1.0);
        glm::vec4 cur_axis_y(tmp_ex_r_mat[0][1], tmp_ex_r_mat[1][1], tmp_ex_r_mat[2][1], 1.0);
        glm::vec4 cur_axis_z(tmp_ex_r_mat[0][2], tmp_ex_r_mat[1][2], tmp_ex_r_mat[2][2], 1.0);
        cur_axis_x = rotate_mat * cur_axis_x;
        cur_axis_y = rotate_mat * cur_axis_y;
        cur_axis_z = rotate_mat * cur_axis_z;
        tmp_ex_r_mat[0][0] = cur_axis_x.x; tmp_ex_r_mat[1][0] = cur_axis_x.y; tmp_ex_r_mat[2][0] = cur_axis_x.z;
        tmp_ex_r_mat[0][1] = cur_axis_y.x; tmp_ex_r_mat[1][1] = cur_axis_y.y; tmp_ex_r_mat[2][1] = cur_axis_y.z;
        tmp_ex_r_mat[0][2] = cur_axis_z.x; tmp_ex_r_mat[1][2] = cur_axis_z.y; tmp_ex_r_mat[2][2] = cur_axis_z.z;

        cur_cam_pos = splitted_center + cur_r * cur_vec;
        tmp_ex_t_mat[3] = glm::vec4(-cur_cam_pos.x, -cur_cam_pos.y, -cur_cam_pos.z, 1.0);
        splitted_cameras.push_back(tmp_ex_r_mat * tmp_ex_t_mat);
    }
}

void mCamera::getSplittedCameras(int camera_num, glm::vec3 pose_center, std::vector<glm::vec3> &splitted_cameras) {
    this->updateViewMat(pose_center);

    splitted_cameras.clear();
    glm::vec3 cur_cam_pos(-this->view_t_mat[3][0], -this->view_t_mat[3][1], -this->view_t_mat[3][2]);
    glm::vec3 splitted_center(pose_center.x, pose_center.y, pose_center.z);

    float per_angle = 2*3.1415927 / camera_num;
    glm::vec3 cur_vec = glm::normalize(cur_cam_pos - splitted_center);
    float cur_r = glm::length(splitted_center - cur_cam_pos);
    glm::mat4 rotate_mat = glm::rotate(glm::mat4(1.f), per_angle, glm::vec3(0, 1, 0));
    for (int i = 0; i < camera_num; ++i) {
        cur_vec = glm::vec3(rotate_mat * glm::vec4(cur_vec, 1.0));
        splitted_cameras.push_back(cur_vec * cur_r);
    }
}
// This funciont can only be used in the follow mode
void mCamera::setVerticalAngle(float angle, glm::vec3 pose_center) {
    this->updateViewMat(pose_center);

    // TODO: Maybe wrong in the ar mode
    glm::vec3 cur_cam_pos(-this->view_t_mat[3][0], -this->view_t_mat[3][1], -this->view_t_mat[3][2]);
    glm::vec3 center_to_cam = glm::normalize(cur_cam_pos - pose_center);
    glm::vec3 rotate_axis = glm::normalize(glm::cross(center_to_cam, glm::vec3(0, 1, 0)));
    glm::vec3 raw_vec = glm::normalize(glm::cross(glm::vec3(0, 1, 0), rotate_axis));
    float length = glm::length(cur_cam_pos - pose_center);

    glm::mat4 rotate_mat = glm::rotate(glm::mat4(1.0f), glm::radians(angle), rotate_axis);
    raw_vec = glm::vec3(rotate_mat * glm::vec4(raw_vec, 1.f));
    this->setViewVec(raw_vec * length);
}

// dir 0 means x, y direction, 1 means z direction
void mCamera::moveCamera(glm::vec3 move_step, glm::vec3 pose_center) {
    this->updateViewMat(pose_center);
    if (this->is_focus) {
        glm::vec3 camera_pos(-this->view_t_mat[3][0], -this->view_t_mat[3][1], -this->view_t_mat[3][2]);
        glm::vec3 raw_vec = camera_pos - pose_center;
        float r = glm::length(raw_vec);
        raw_vec /= r; // normalize

        glm::vec3 angles = move_step / 30.0f;
        raw_vec = glm::vec3(glm::rotate(glm::mat4(1.f), angles.x, glm::vec3(0, 1, 0)) * glm::vec4(raw_vec, 1.0));//  axis y
        glm::vec3 cross_axis = glm::cross(raw_vec, glm::vec3(0, 1, 0));
        raw_vec = glm::vec3(glm::rotate(glm::mat4(1.f), angles.y, cross_axis) * glm::vec4(raw_vec, 1.0));

        this->setViewVec(raw_vec * (r + move_step.z));
        this->updateViewMat(pose_center);
    }
    else {
        glm::vec3 dir_x(this->view_r_mat[0][0], this->view_r_mat[1][0], this->view_r_mat[2][0]);
        glm::vec3 dir_y(this->view_r_mat[0][1], this->view_r_mat[1][1], this->view_r_mat[2][1]);
        glm::vec3 dir_z(-this->view_r_mat[0][2], -this->view_r_mat[1][2], -this->view_r_mat[2][2]);

        this->view_t_mat = glm::translate(glm::translate(glm::translate(this->view_t_mat, move_step.x * dir_x), move_step.y * dir_y), move_step.z * dir_z);
        this->updateViewVec(pose_center);
    }
}

void mCamera::rotateCamera(glm::mat4 rotate_mat) {
    this->view_r_mat = this->view_r_mat * rotate_mat;
}

bool mCamera::getFocus() const {
    return this->is_focus;
}

bool mCamera::getFollow() const {
    return this->is_follow;
}

int mCamera::getCameraType() const {
    return this->camera_type;
}

glm::vec3 mCamera::getCameraPos(glm::vec3 pose_center) const {
    glm::mat4 cur_view_t_mat, cur_view_r_mat;
    this->getViewMat(pose_center, &cur_view_r_mat, &cur_view_t_mat);
    return glm::vec3(-cur_view_t_mat[3][0], -cur_view_t_mat[3][1], -cur_view_t_mat[3][2]);
}

// To display the camera pos
glm::mat4 mCamera::getAffineMatrix(glm::vec3 pose_center) const {
    float e = 0.00005;

    glm::mat4 cur_view_t_mat, cur_view_r_mat;
    this->getViewMat(pose_center, &cur_view_r_mat, &cur_view_t_mat);
    glm::vec3 camera_pos(-cur_view_t_mat[3][0], -cur_view_t_mat[3][1], -cur_view_t_mat[3][2]);
    glm::mat4 rotate_mat;

    glm::vec3 raw_axis_x = glm::vec3(1, 0, 0);
    glm::vec3 raw_axis_y = glm::vec3(0, 1, 0);
    glm::vec3 cur_axis_x = glm::vec3(cur_view_r_mat[0][0], cur_view_r_mat[1][0], cur_view_r_mat[2][0]);
    glm::vec3 cur_axis_y = glm::vec3(cur_view_r_mat[0][1], cur_view_r_mat[1][1], cur_view_r_mat[2][1]);

    // first rotate the coordiant to make the axis_x in the new axis_x
    glm::vec3 rotate_axis;
    float rotate_angle;

    if (glm::length(raw_axis_x - cur_axis_x) < e) {
        // the vector is in the same line
        rotate_mat = glm::mat4(1.f);
    }
    else {
        rotate_axis = glm::normalize(glm::cross(raw_axis_x, cur_axis_x));
        rotate_angle = glm::acos(glm::dot(raw_axis_x, cur_axis_x));
        rotate_mat = glm::rotate(glm::mat4(1.f), rotate_angle, rotate_axis);
    }

    raw_axis_x = cur_axis_x;
    raw_axis_y = glm::vec3(rotate_mat * glm::vec4(raw_axis_y, 1.0f));

    if (glm::length(raw_axis_y - cur_axis_y) >= e) {
        rotate_axis = glm::normalize(glm::cross(raw_axis_y, cur_axis_y));
        rotate_angle = glm::acos(glm::dot(raw_axis_y, cur_axis_y));
        rotate_mat = glm::rotate(glm::mat4(1.f), rotate_angle, rotate_axis) * rotate_mat;
    }

    return glm::translate(glm::mat4(1.f), camera_pos - glm::vec3(0.f)) * rotate_mat;
}
