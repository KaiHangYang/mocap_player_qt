#include "mPoseModel.h"
#include <cstring>
#include "mPoseDefs.h"
#include "mRenderParameters.h"
#include <glm/glm.hpp>

mPoseModel::mPoseModel(QOpenGLVertexArrayObject * vao, QOpenGLFunctions_3_3_Core * core_func, mShader * pose_shader, mShader * depth_shader, float target_model_size, bool is_ar, bool & use_shading, int pose_type): use_shading(use_shading) {

    this->pose_shader = pose_shader;
    this->depth_shader = depth_shader;

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

    /************** State parameters ***************/
    this->is_ar = is_ar;
}

mPoseModel::~mPoseModel() {
    this->mesh_reader->~mMeshReader();
}

bool joint_z_cmp(std::pair<glm::vec3, int> a, std::pair<glm::vec3, int> b) {
    if (std::abs(a.first.z) < std::abs(b.first.z)) {
        return false;
    }
    else {
        return true;
    }
}

void mPoseModel::renderPose(std::vector<glm::vec3> raw_vertexs, std::vector<glm::vec3> vertexs, glm::mat4 view_mat, glm::mat4 proj_mat, int camera_type, int render_type) {

    this->VAO->bind();
    unsigned int vertexNum = vertexs.size();
    unsigned int lineNum = this->bone_indices.size();
    std::vector<std::pair<glm::vec3, int>> camera_coord_vertexs(vertexs.size(), std::pair<glm::vec3, int>(glm::vec3(0), 0));

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
    shader->setVal("view", view_mat);

    glm::mat4 trans;
    glm::mat4 curmodel;

    /*****************************change the joints color according to the position********************************/
    // the raw_vertex is used for calcualte.

    std::vector<glm::vec3> tmpJointColors = mRenderParams::mJointColors;
    if (this->use_shading) {
        tmpJointColors[14] = glm::vec3(1.f);
        // use the hip bone as the ruler
        float relative_position_threshhold = ((glm::length(raw_vertexs[mPoseDef::left_hip_joint_index] - raw_vertexs[mPoseDef::root_of_joints]) + glm::length(raw_vertexs[mPoseDef::right_hip_joint_index] - raw_vertexs[mPoseDef::root_of_joints])) / 2) * 0.1;
        for (unsigned int i = 0; i < lineNum; ++i) {
            unsigned int line[2] = { this->bone_indices[i].x, this->bone_indices[i].y };
            if (!vertexFlags[line[1]]) {
                vertexFlags[line[1]] = true;
                glm::mat4 to_camera_coord = view_mat;

                if (render_type == 0) {
                    // the vertex is in the global coordinate, so I only need to use the view_mat
                    camera_coord_vertexs[line[0]].first = to_camera_coord * glm::vec4(raw_vertexs[line[0]], 1.f);
                    camera_coord_vertexs[line[0]].second = line[0];
                    camera_coord_vertexs[line[1]].first = to_camera_coord * glm::vec4(raw_vertexs[line[1]], 1.f);
                    camera_coord_vertexs[line[1]].second = line[1];
                    float dert_z = std::abs((camera_coord_vertexs[line[0]].first).z) - std::abs((camera_coord_vertexs[line[1]].first).z);

                    if (dert_z > relative_position_threshhold) {
                        tmpJointColors[line[1]] = glm::vec3(1.f);
                    }
                    else if (dert_z < -relative_position_threshhold) {
                        tmpJointColors[line[1]] = glm::vec3(0.f);
                    }
                    else {
                        tmpJointColors[line[1]] = glm::vec3(0.5f);
                    }
                }
            }
        }
    }
    /*********************************************************************************/

    std::sort(camera_coord_vertexs.begin(), camera_coord_vertexs.end(), joint_z_cmp);
    // Draw the bones first
    this->core_func->glEnable(GL_DEPTH_TEST);

    for (unsigned int i = 0; i < lineNum; ++i) {
        unsigned int line[2] = { this->bone_indices[i].x, this->bone_indices[i].y };

        glm::vec3 joint_source = vertexs[line[0]];
        glm::vec3 joint_target = vertexs[line[1]];

        glm::vec3 lineCen = (joint_source + joint_target) / 2.f;
        float length = glm::length(joint_source - joint_target);

        glm::vec3 vFrom(0, 1, 0);
        glm::vec3 vTo = glm::normalize(joint_source - joint_target);

        trans = glm::translate(glm::mat4(1.0), lineCen);
        float angle = (float)glm::acos(glm::dot(vFrom, vTo));

        if (angle <= 0.0000001) {
            curmodel = glm::mat4(1.0f);
        }
        else {
            curmodel = glm::rotate(glm::mat4(1.0), angle, glm::normalize(glm::cross(vFrom, vTo)));
        }
        // TODO: Temporary scale the bones and the dot
        glm::mat4 scaleMat = glm::scale(glm::mat4(1.0), glm::vec3(1.0f * mRenderParams::skeleton_style[2*i + 1], length/this->model_size, 1.0f * mRenderParams::skeleton_style[2*i + 1])) * glm::scale(glm::mat4(1.0), glm::vec3(this->model_scale));

        curmodel = trans * curmodel * scaleMat;
        
        shader->setVal("model", curmodel);

        if (render_type == 0) {
            shader->setVal("fragColor", mRenderParams::mBoneColors[i]);
            shader->setVal("normMat", glm::transpose(glm::inverse(curmodel)));
        }
        this->mesh_reader->render(mRenderParams::skeleton_style[2*i]);
    }

    // Then draw the joints
    // TODO: TO BE MODIFIED, what a mess now
    /*********************** Draw the root first ***************************/
//    if (!this->use_shading) {
//        this->core_func->glDisable(GL_DEPTH_TEST);
//    }

    for (unsigned int i = 0; i < vertexNum; ++i) {
        int cur_vertex_num = i;

        curmodel = glm::scale(glm::mat4(1.f), 1.0f * this->model_scale * glm::vec3(mPoseDef::model_joint_bone_ratio, mPoseDef::model_joint_bone_ratio, mPoseDef::model_joint_bone_ratio));
//        curmodel = glm::scale(glm::mat4(1.f), 1.0f * this->model_scale * glm::vec3(1.f));
        curmodel = glm::translate(glm::mat4(1.0f), vertexs[cur_vertex_num]) * curmodel;
        shader->setVal("model", curmodel);
        if (render_type == 0) {
            shader->setVal("fragColor", tmpJointColors[cur_vertex_num]);
            shader->setVal("normMat", glm::transpose(glm::inverse(curmodel)));
        }
        if (i == mPoseDef::neck_joint_index || i == mPoseDef::root_of_joints) {
            // Don't draw the neck and root joint
            continue;
        }
        else {
            mesh_reader->render(0);
        }

    }
}

void mPoseModel::draw(std::vector<glm::vec3> raw_joints, std::vector<glm::vec3> points, glm::mat4 & cam_ex_mat, glm::mat4 & cam_in_mat, int camera_type, int render_type) {
    this->renderPose(raw_joints, points, cam_ex_mat, cam_in_mat, camera_type, render_type);
}
