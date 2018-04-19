#include "mPoseAdjuster.h"
#include <QString>
#include <QList>

mPoseAdjuster::mPoseAdjuster(const std::vector<float> & bones_length, const std::vector<unsigned int> & bones_length_index, const std::vector<glm::u32vec2> & bones_indices, const std::vector<unsigned int> bones_cal_rank): pose_bones_length(bones_length), pose_bones_indices(bones_indices), pose_bones_cal_rank(bones_cal_rank), pose_bones_length_index(bones_length_index) {
    this->rand_gen = QRandomGenerator::system();
    this->pose_jitter_bones_length = this->pose_bones_length;
}
mPoseAdjuster::~mPoseAdjuster() {}

void mPoseAdjuster::setBonesLengthJitters(float jitter_size) {
    float random_jitter;
    for (int i = 0; i < this->pose_jitter_bones_length.size(); ++i) {
        random_jitter = 1 - this->rand_gen->bounded(2.0);
        this->pose_jitter_bones_length[i] = this->pose_bones_length[i] * (1 - jitter_size * random_jitter);
    }
}

void mPoseAdjuster::adjustAccordingToBoneLength(std::vector<glm::vec3> &joints, bool with_jitters) {
    std::vector<float> bones_length;
    this->calBonesLength(joints, bones_length);

    std::vector<glm::vec3> bones_vec_arr(this->pose_bones_indices.size());
    std::vector<float> cur_bones_length;

    if (with_jitters) {
        cur_bones_length = this->pose_jitter_bones_length;
    }
    else {
        cur_bones_length = this->pose_bones_length;
    }

    // TODO add random and set the jitter to be a state
    for (int i = 0; i < this->pose_bones_indices.size(); ++i) {
        glm::i32vec2 cur_bone = this->pose_bones_indices[i];
        bones_vec_arr[i] = glm::normalize(joints[cur_bone.y] - joints[cur_bone.x]);
    }

    // the first and fixed point is the root point
    for (int i = 0; i < this->pose_bones_cal_rank.size(); ++i) {
        int cur_bone_index = this->pose_bones_cal_rank[i];
        glm::i32vec2 cur_bone = this->pose_bones_indices[cur_bone_index];
        joints[cur_bone.y] = joints[cur_bone.x] + cur_bones_length[this->pose_bones_length_index[cur_bone_index]] * bones_vec_arr[cur_bone_index];
    }
}

void mPoseAdjuster::calBonesLength(const std::vector<glm::vec3> & joints, std::vector<float> & bones_length) {
    bones_length = std::vector<float>(this->pose_bones_indices.size());
    for (int i = 0; i < this->pose_bones_indices.size(); ++i) {
        glm::i32vec2 cur_bone = this->pose_bones_indices[i];
        bones_length[i] = glm::length(joints[cur_bone.x] - joints[cur_bone.y]);
    }
    qDebug() << "spin / upper arm" <<  bones_length[7] / ((bones_length[2] + bones_length[5]) / 2.0);
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
