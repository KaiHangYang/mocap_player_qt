#include "mPoseAdjuster.h"
#include <QString>
#include <QList>

#include "mIKOpt.h"
// The header is used for checking whether the render mode is ar mode

mPoseAdjuster::mPoseAdjuster(const std::vector<float> & bones_length, const std::vector<unsigned int> & bones_length_index, const std::vector<glm::u32vec2> & bones_indices, const std::vector<unsigned int> bones_cal_rank): pose_bones_length(bones_length), pose_bones_indices(bones_indices), pose_bones_cal_rank(bones_cal_rank), pose_bones_length_index(bones_length_index) {
    this->rand_gen = QRandomGenerator::system();
    this->pose_jitter_bones_length = this->pose_bones_length;
}
mPoseAdjuster::~mPoseAdjuster() {}

glm::vec3 mPoseAdjuster::jitterParams(std::vector<double> &params, float angle_jitter_range) {
    // discard
    int bones_num = params.size() / 3 - 1;
    double * angle_ptr = &params[3]; // discard the global rotate angles
    // discard the shoulder and the hip bones
    std::vector<int> discard_index({1, 4, 8, 11});
    for (int i = 0; i < bones_num; ++i) {
        if (i == discard_index[0] || i == discard_index[1] || i == discard_index[2] || i == discard_index[3]) {
            continue;
        }
        else {
            angle_ptr[3*i + 0] = angle_ptr[3*i + 0] +  3.1415026 * (angle_jitter_range * (this->rand_gen->bounded(2.00001) - 1));
            angle_ptr[3*i + 1] = angle_ptr[3*i + 1] +  3.1415026 * (angle_jitter_range * (this->rand_gen->bounded(2.00001) - 1));
            angle_ptr[3*i + 2] = angle_ptr[3*i + 2] +  3.1415026 * (angle_jitter_range * (this->rand_gen->bounded(2.00001) - 1));
        }
    }
}

void mPoseAdjuster::setBonesLengthJitters(float jitter_size) {
    float random_jitter;
    for (int i = 0; i < this->pose_jitter_bones_length.size(); ++i) {
        random_jitter = 1 - this->rand_gen->bounded(2.0);
        this->pose_jitter_bones_length[i] = this->pose_bones_length[i] * (1 - jitter_size * random_jitter);
    }
}

void mPoseAdjuster::adjustAccordingToBoneLength(std::vector<glm::vec3> &joints, float jitter_range, float angle_jitter_range) {

    std::vector<glm::vec3> bones_vec_arr(this->pose_bones_indices.size());
    std::vector<float> cur_bones_length;
    cur_bones_length = this->pose_bones_length;

    /************************** My way to calculate the bones *****************************/
//     TODO add random and set the jitter to be a state
    for (int i = 0; i < this->pose_bones_indices.size(); ++i) {
        glm::i32vec2 cur_bone = this->pose_bones_indices[i];
        bones_vec_arr[i] = glm::normalize(joints[cur_bone.y] - joints[cur_bone.x]);
    }

    // use some jitter in the cur_bone_length
    for (int i = 0; i < cur_bones_length.size(); ++i) {
        cur_bones_length[i] = (1 + jitter_range * (this->rand_gen->bounded(2.00001) - 1)) * cur_bones_length[i];
    }
    // the first and fixed point is the root point
    for (int i = 0; i < this->pose_bones_cal_rank.size(); ++i) {
        int cur_bone_index = this->pose_bones_cal_rank[i];
        glm::i32vec2 cur_bone = this->pose_bones_indices[cur_bone_index];
        joints[cur_bone.y] = joints[cur_bone.x] + cur_bones_length[this->pose_bones_length_index[cur_bone_index]] * bones_vec_arr[cur_bone_index];
    }
    // Then jitter the euler angle
    std::vector<double> joints_dbl(joints.size() * 3, 0);
    std::vector<double> cur_bones_length_dbl(cur_bones_length.size(), 0);
    glm::vec3 root_joint = joints[joints.size() - 1];
    for (int i = 0; i < joints.size(); ++i) {
        joints_dbl[3*i+0] = joints[i].x - root_joint.x;
        joints_dbl[3*i+1] = joints[i].y - root_joint.y;
        joints_dbl[3*i+2] = joints[i].z - root_joint.z;
    }
    std::copy(cur_bones_length.begin(), cur_bones_length.end(), cur_bones_length_dbl.begin());
    std::vector<double> params = mIKOpt::optimizeIK(joints_dbl, cur_bones_length_dbl);
    // put jitters to the euler angles
    this->jitterParams(params, angle_jitter_range);
    joints_dbl = mIKOpt::points_from_angles<double>(&params[0], cur_bones_length_dbl);
    for (int i = 0; i < joints.size(); ++i) {
        joints[i] = glm::vec3(joints_dbl[3*i+0] + root_joint.x, joints_dbl[3*i+1] + root_joint.y, joints_dbl[3*i+2] + root_joint.z);
    }

}

void mPoseAdjuster::calBonesLength(const std::vector<glm::vec3> & joints, std::vector<float> & bones_length) {
    bones_length = std::vector<float>(this->pose_bones_indices.size());
    for (int i = 0; i < this->pose_bones_indices.size(); ++i) {
        glm::i32vec2 cur_bone = this->pose_bones_indices[i];
        bones_length[i] = glm::length(joints[cur_bone.x] - joints[cur_bone.y]);
    }
//    qDebug() << "spin / upper arm" <<  bones_length[7] / ((bones_length[2] + bones_length[5]) / 2.0);
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
