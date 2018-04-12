#include "mPoseAdjuster.h"
#include <QString>
#include <QList>

void mPoseAdjuster::adjustAccordingToBoneLength(std::vector<glm::vec3> &joints, bool with_jitter) {
    std::vector<glm::vec3> bones_vec_arr(this->pose_bones_indices.size());

    for (int i = 0; i < this->pose_bones_indices.size(); ++i) {
        glm::i32vec2 cur_bone = this->pose_bones_indices[i];
        bones_vec_arr[i] = glm::normalize(joints[cur_bone.y] - joints[cur_bone.x]);
    }
    // the first and fixed point is the root point
    for (int i = 0; i < this->pose_bones_cal_rank.size(); ++i) {
        int cur_bone_index = this->pose_bones_cal_rank[i];
        glm::i32vec2 cur_bone = this->pose_bones_indices[cur_bone_index];
        joints[cur_bone.y] = joints[cur_bone.x] + this->pose_bones_length[cur_bone_index] * bones_vec_arr[cur_bone_index];
    }
}

void mPoseAdjuster::calBonesLength(const std::vector<glm::vec3> & joints, std::vector<float> & bones_length) {
    bones_length = std::vector<float>(this->pose_bones_indices.size());
    for (int i = 0; i < this->pose_bones_indices.size(); ++i) {
        glm::i32vec2 cur_bone = this->pose_bones_indices[i];
        bones_length[i] = glm::length(joints[cur_bone.x] - joints[cur_bone.y]);
    }
}

void mPoseAdjuster::printBonesLength(const std::vector<float> & bones_length) {
    std::vector<float> display_arr;
    if (bones_length.size() == this->pose_bones_indices.size()) {
        display_arr = bones_length;
    }
    else {
        display_arr = this->pose_bones_length;
    }

    for (int i = 0; i < display_arr.size(); ++i) {
        qDebug() << "Bone: " << i << " Length: " << display_arr[i];
    }
}
