#include "mPoseModel.h"
#include <cstring>
#include "mPoseDefs.h"
#include "mRenderParameters.h"

static float skeleton_style[] = {
    0, 1.3, // head
    1, 0.6, // left shoulder
    0, 1.5, // left upper arm
    0, 1.3, // left lower arm
    1, 0.6, // right shoulder
    0, 1.5, // right upper arm
    0, 1.3, // right lower arm
    1, 0.6, // spine
    1, 0.6, // left hip
    0, 2.0, // left ham,
    0, 1.6, // left calf,
    1, 0.6, // right hip
    0, 2.0, // right ham,
    0, 1.6, // right calf,
    0, 1.3, // left feet,
    0, 1.3, // right feet
};

mPoseModel::mPoseModel(QOpenGLVertexArrayObject * vao, QOpenGLFunctions_3_3_Core * core_func, mShader * pose_shader, mShader * depth_shader, glm::mat4 cam_in_mat, float target_model_size, bool is_ar, int pose_type) {

    this->pose_adjuster = new mPoseAdjuster(mPoseDef::bones_length, mPoseDef::bones_length_index, mPoseDef::bones_indices,mPoseDef::bones_cal_rank);
    this->is_use_jitter = false;

    this->pose_shader = pose_shader;
    this->depth_shader = depth_shader;

    this->proj_mat = cam_in_mat;
    this->is_ar = is_ar;
    this->core_func = core_func;

    this->VAO = vao;

    if (pose_type == 0) {
        this->num_of_joints = mPoseDef::num_of_joints;
        this->bone_indices = mPoseDef::bones_indices;
    }
    else {
        std::cout << "(mPoseModel.cpp) pose_type is not valid!" << std::endl;
        exit(-1);
    }

    this->model_size = mPoseDef::model_size;
    this->model_scale = target_model_size / this->model_size;
    this->model_size = target_model_size;

    this->mesh_reader = new mMeshReader(this->VAO, this->core_func);
    this->mesh_reader->addMesh(mPoseDef::model_base_dir + "sphere-30.ply");
    this->mesh_reader->addMesh(mPoseDef::model_base_dir + "cylinder-30.ply");
}

mPoseModel::~mPoseModel() {
    this->mesh_reader->~mMeshReader();
    this->pose_adjuster->~mPoseAdjuster();
}

void mPoseModel::renderPose(std::vector<glm::vec3> &vertexs, glm::mat4 view_mat, int render_type) {

    this->VAO->bind();
    unsigned int vertexNum = vertexs.size();
    unsigned int lineNum = this->bone_indices.size();

    std::vector<bool> vertexFlags(vertexNum, false);
    glm::mat4 view_r_mat = glm::mat4(glm::mat3(view_mat));
    glm::mat4 view_t_mat = glm::inverse(view_r_mat) * view_mat;

    mShader * shader;
    if (render_type == 0) {
        shader = this->pose_shader;
    }
    else if (render_type == 1)  {
        shader = this->depth_shader;
    }
    else {
        std::cout << "(mPoseModel.cpp): render_type is not valid!" << std::endl;
        exit(-1);
    }
    shader->use();

    // Some uniform has been set in the scene_utils before this function is called
    shader->setVal("renderType", 0);
    shader->setVal("viewPos", glm::vec3(-view_t_mat[3][0], -view_t_mat[3][1], -view_t_mat[3][2]));
    shader->setVal("fragColor", mBoneColor);
    shader->setVal("projection", this->proj_mat);
    shader->setVal("view", view_mat);

    glm::u32vec2 * indices_ptr = &this->bone_indices[0];
    glm::mat4 trans;
    glm::mat4 curmodel;

    for (unsigned int i = 0; i < lineNum; ++i) {
        unsigned int line[2] = { indices_ptr->x, indices_ptr->y };
        indices_ptr ++;
        // Draw the points first
        for (unsigned int j = 0; j < 2; ++j) {
            if (!vertexFlags[line[j]]) {
                vertexFlags[line[j]] = true;

                curmodel = glm::scale(glm::mat4(1.f), this->model_scale * glm::vec3(0.8, 0.8, 0.8));
                curmodel = glm::translate(glm::mat4(1.0f), vertexs[line[j]]) * curmodel;

                shader->setVal("model", curmodel);
                if (render_type == 0) {
                    shader->setVal("normMat", glm::transpose(glm::inverse(curmodel)));
                }
                mesh_reader->render(0);
            }
        }

        glm::vec3 lineCen = (vertexs[line[0]] + vertexs[line[1]]) / 2.f;
        float length = glm::length(vertexs[line[0]] - vertexs[line[1]]);

        glm::vec3 vFrom(0, 1, 0);
        glm::vec3 vTo = glm::normalize(vertexs[line[0]] - vertexs[line[1]]);

        trans = glm::translate(glm::mat4(1.0), lineCen);
        float angle = (float)glm::acos(glm::dot(vFrom, vTo));

        if (angle <= 0.0000001) {
            curmodel = glm::mat4(1.0f);
        }
        else {
            curmodel = glm::rotate(glm::mat4(1.0), angle, glm::normalize(glm::cross(vFrom, vTo)));
        }
        glm::mat4 scaleMat = glm::scale(glm::mat4(1.0), glm::vec3(skeleton_style[2*i + 1], length/this->model_size, skeleton_style[2*i + 1])) * glm::scale(glm::mat4(1.0), glm::vec3(this->model_scale));

        curmodel = trans * curmodel * scaleMat;
        
        shader->setVal("model", curmodel);

        if (render_type == 0) {
            shader->setVal("fragColor", mBoneColor);
            shader->setVal("normMat", glm::transpose(glm::inverse(curmodel)));
        }

        this->mesh_reader->render(skeleton_style[2*i]);
    }
}

void mPoseModel::setJitterPose(bool is_use_jitter, float jitter_size) {
    this->is_use_jitter = is_use_jitter;
    if (this->is_use_jitter) {
        this->pose_adjuster->setBonesLengthJitters(jitter_size);
    }
}
void mPoseModel::draw(std::vector<glm::vec3> points, glm::mat4 raw_cam_ex_mat_inv, glm::mat4 & cam_ex_mat, int render_type) {
    this->pose_adjuster->adjustAccordingToBoneLength(points, this->is_use_jitter);
    int p_num = points.size();

    glm::vec3 * p_ptr = &points[0];
    glm::vec4 p_cur;

    for (int i = 0; i < p_num; ++i) {
        p_cur = glm::vec4(*p_ptr, 1.0);

        p_cur = raw_cam_ex_mat_inv * p_cur;

        p_ptr->x = p_cur[0];
        p_ptr->y = p_cur[1];
        p_ptr->z = p_cur[2];
        p_ptr++;
    }
    this->renderPose(points, cam_ex_mat, render_type);
}
