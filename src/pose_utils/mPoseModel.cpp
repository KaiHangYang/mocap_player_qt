#include "mPoseModel.h"
#include <cstring>
#include "mPoseDefs.h"
#include "mRenderParameters.h"
#include <glm/glm.hpp>

//static float skeleton_style[] = {
//    0, 1.3, // head
//    1, 0.6, // left shoulder
//    0, 1.5, // left upper arm
//    0, 1.3, // left lower arm
//    1, 0.6, // right shoulder
//    0, 1.5, // right upper arm
//    0, 1.3, // right lower arm
//    1, 0.6, // spine
//    1, 0.6, // left hip
//    0, 2.0, // left ham,
//    0, 1.6, // left calf,
//    1, 0.6, // right hip
//    0, 2.0, // right ham,
//    0, 1.6, // right calf,
//    0, 1.3, // left feet,
//    0, 1.3, // right feet
//};
static float skeleton_style[] = {
    1, 1.0, // head
    1, 1.0, // left shoulder
    1, 1.0, // left upper arm
    1, 1.0, // left lower arm
    1, 1.0, // right shoulder
    1, 1.0, // right upper arm
    1, 1.0, // right lower arm
    1, 1.0, // spine
    1, 1.0, // left hip
    1, 1.0, // left ham,
    1, 1.0, // left calf,
    1, 1.0, // right hip
    1, 1.0, // right ham,
    1, 1.0, // right calf,
    1, 1.0, // left feet,
    1, 1.0, // right feet
};
//static glm::vec3 mBoneColors[] = {
    //glm::vec3(0.5294117647058824, 0.9882352941176471, 0.788235294117647), // head
    //glm::vec3(0.5294117647058824, 0.788235294117647, 0.9882352941176471), // left shoulder
    //glm::vec3(0.5294117647058824, 0.788235294117647, 0.9882352941176471), // left upper arm
    //glm::vec3(0.5294117647058824, 0.788235294117647, 0.9882352941176471), // left lower arm
    //glm::vec3(0.9882352941176471, 0.788235294117647, 0.5294117647058824), // right shoulder
    //glm::vec3(0.9882352941176471, 0.788235294117647, 0.5294117647058824), // right upper arm
    //glm::vec3(0.9882352941176471, 0.788235294117647, 0.5294117647058824), // right lower arm
    //glm::vec3(0.5294117647058824, 0.9882352941176471, 0.788235294117647), // spine
    //glm::vec3(0.5294117647058824, 0.788235294117647, 0.9882352941176471), // left hip
    //glm::vec3(0.5294117647058824, 0.788235294117647, 0.9882352941176471), // left ham,
    //glm::vec3(0.5294117647058824, 0.788235294117647, 0.9882352941176471), // left calf,
    //glm::vec3(0.9882352941176471, 0.788235294117647, 0.5294117647058824), // right hip
    //glm::vec3(0.9882352941176471, 0.788235294117647, 0.5294117647058824), // right ham,
    //glm::vec3(0.9882352941176471, 0.788235294117647, 0.5294117647058824), // right calf,
    //glm::vec3(0.5294117647058824, 0.788235294117647, 0.9882352941176471), // left feet,
    //glm::vec3(0.9882352941176471, 0.788235294117647, 0.5294117647058824) // right feet
//};

//static glm::vec3 mJointColors[] = {
    //glm::vec3(0.5294117647058824, 0.9882352941176471, 0.788235294117647), // head
    //glm::vec3(0.5294117647058824, 0.9882352941176471, 0.788235294117647), // neck

    //glm::vec3(0.5294117647058824, 0.788235294117647, 0.9882352941176471), // left shoulder
    //glm::vec3(0.5294117647058824, 0.788235294117647, 0.9882352941176471), // left upper arm
    //glm::vec3(0.5294117647058824, 0.788235294117647, 0.9882352941176471), // left lower arm
    //glm::vec3(0.9882352941176471, 0.788235294117647, 0.5294117647058824), // right shoulder
    //glm::vec3(0.9882352941176471, 0.788235294117647, 0.5294117647058824), // right upper arm
    //glm::vec3(0.9882352941176471, 0.788235294117647, 0.5294117647058824), // right lower arm

    //glm::vec3(0.5294117647058824, 0.788235294117647, 0.9882352941176471), // left hip
    //glm::vec3(0.5294117647058824, 0.788235294117647, 0.9882352941176471), // left ham,
    //glm::vec3(0.5294117647058824, 0.788235294117647, 0.9882352941176471), // left calf,
    //glm::vec3(0.9882352941176471, 0.788235294117647, 0.5294117647058824), // right hip
    //glm::vec3(0.9882352941176471, 0.788235294117647, 0.5294117647058824), // right ham,
    //glm::vec3(0.9882352941176471, 0.788235294117647, 0.5294117647058824), // right calf,

    //glm::vec3(0.5294117647058824, 0.9882352941176471, 0.788235294117647), // root ,

    //glm::vec3(0.5294117647058824, 0.788235294117647, 0.9882352941176471), // left feet,
    //glm::vec3(0.9882352941176471, 0.788235294117647, 0.5294117647058824) // right feet
//};

static glm::vec3 mBoneColors[] = {
    glm::vec3(0, 1, 0), // head
    glm::vec3(1, 0, 0), // left shoulder
    glm::vec3(1, 0, 0), // left upper arm
    glm::vec3(1, 0, 0), // left lower arm
    glm::vec3(0, 0.6, 1), // right shoulder
    glm::vec3(0, 0.6, 1), // right upper arm
    glm::vec3(0, 0.6, 1), // right lower arm
    glm::vec3(0, 1, 0), // spine
    glm::vec3(1, 0, 0), // left hip
    glm::vec3(1, 0, 0), // left ham,
    glm::vec3(1, 0, 0), // left calf,
    glm::vec3(0, 0.6, 1), // right hip
    glm::vec3(0, 0.6, 1), // right ham,
    glm::vec3(0, 0.6, 1), // right calf,
    glm::vec3(1, 0, 0), // left feet,
    glm::vec3(0, 0.6, 1) // right feet
};

static glm::vec3 mJointColors[] = {
    glm::vec3(0, 1, 0), // head
    glm::vec3(0, 1, 0), // neck

    glm::vec3(1, 0, 0), // left shoulder
    glm::vec3(1, 0, 0), // left upper arm
    glm::vec3(1, 0, 0), // left lower arm
    glm::vec3(0, 0.6, 1), // right shoulder
    glm::vec3(0, 0.6, 1), // right upper arm
    glm::vec3(0, 0.6, 1), // right lower arm

    glm::vec3(1, 0, 0), // left hip
    glm::vec3(1, 0, 0), // left ham,
    glm::vec3(1, 0, 0), // left calf,
    glm::vec3(0, 0.6, 1), // right hip
    glm::vec3(0, 0.6, 1), // right ham,
    glm::vec3(0, 0.6, 1), // right calf,

    glm::vec3(0, 1, 0), // root ,

    glm::vec3(1, 0, 0), // left feet,
    glm::vec3(0, 0.6, 1) // right feet
};

mPoseModel::mPoseModel(QOpenGLVertexArrayObject * vao, QOpenGLFunctions_3_3_Core * core_func, mShader * pose_shader, mShader * depth_shader, glm::mat4 cam_in_mat, float target_model_size, bool is_ar, int pose_type) {

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

                curmodel = glm::scale(glm::mat4(1.f), this->model_scale * glm::vec3(1.3, 1.3, 1.3));
                curmodel = glm::translate(glm::mat4(1.0f), vertexs[line[j]]) * curmodel;

                shader->setVal("model", curmodel);
                if (render_type == 0) {
                    shader->setVal("fragColor", mJointColors[line[j]]);
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
            shader->setVal("fragColor", mBoneColors[i]);
            shader->setVal("normMat", glm::transpose(glm::inverse(curmodel)));
        }

        this->mesh_reader->render(skeleton_style[2*i]);
    }
}

void mPoseModel::draw(std::vector<glm::vec3> points, glm::mat4 & cam_ex_mat, int render_type) {
    this->renderPose(points, cam_ex_mat, render_type);
}
