#include "mPoseAdjuster.h"
#include <QString>
#include <QList>
#include <QRandomGenerator>
#include <QTime>

void mPoseAdjuster::adjustAccordingToBoneLength(std::vector<glm::vec3> &joints, bool with_jitters, float jitter_size) {
    std::vector<glm::vec3> bones_vec_arr(this->pose_bones_indices.size());
    std::vector<float> cur_bones_length = this->pose_bones_length;
    // TODO add random and set the jitter to be a state

    float random_jitter = 0.0;

    for (int i = 0; i < this->pose_bones_indices.size(); ++i) {
        glm::i32vec2 cur_bone = this->pose_bones_indices[i];
        bones_vec_arr[i] = glm::normalize(joints[cur_bone.y] - joints[cur_bone.x]);
    }
    if (with_jitters) {
        for (int i = 0; cur_bones_length.size(); ++i) {
            cur_bones_length[i] *= (1 - jitter_size * random_jitter);
        }
    }
    // the first and fixed point is the root point
    for (int i = 0; i < this->pose_bones_cal_rank.size(); ++i) {
        int cur_bone_index = this->pose_bones_cal_rank[i];
        glm::i32vec2 cur_bone = this->pose_bones_indices[cur_bone_ndex];
        joints[cur_bone.y] = joints[cur_bone.x] + cur_bones_length[this->pose_bones_length_index[cur_bone_index]] * bones_vec_arr[cur_bone_index];
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
