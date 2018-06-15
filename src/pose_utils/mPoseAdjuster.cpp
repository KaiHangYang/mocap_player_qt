#include "mPoseAdjuster.h"
#include <QString>
#include <QList>

#include "mOurOpt.h"
// The header is used for checking whether the render mode is ar mode
#include "mRenderParameters.h"

mPoseAdjuster::mPoseAdjuster(const std::vector<float> & bones_length, const std::vector<unsigned int> & bones_length_index, const std::vector<glm::u32vec2> & bones_indices, const std::vector<unsigned int> bones_cal_rank): pose_bones_length(bones_length), pose_bones_indices(bones_indices), pose_bones_cal_rank(bones_cal_rank), pose_bones_length_index(bones_length_index) {
    this->rand_gen = QRandomGenerator::system();
    this->pose_jitter_bones_length = this->pose_bones_length;
    this->jitter_range = 0;
}
mPoseAdjuster::~mPoseAdjuster() {}

void mPoseAdjuster::setBonesLengthJitters(float jitter_size) {
    float random_jitter;
    for (int i = 0; i < this->pose_jitter_bones_length.size(); ++i) {
        random_jitter = 1 - this->rand_gen->bounded(2.0);
        this->pose_jitter_bones_length[i] = this->pose_bones_length[i] * (1 - jitter_size * random_jitter);
    }
}

void mPoseAdjuster::adjustAccordingToBoneLength(std::vector<glm::vec3> &joints, float jitter_range) {

    std::vector<glm::vec3> bones_vec_arr(this->pose_bones_indices.size());
    std::vector<float> cur_bones_length;
    cur_bones_length = this->pose_bones_length;

    // TODO: Waiting for implementation for the adjustor!!!!!!!!!!
    /************************** My way to calculate the bones *****************************/
//     TODO add random and set the jitter to be a state
    for (int i = 0; i < this->pose_bones_indices.size(); ++i) {
        glm::i32vec2 cur_bone = this->pose_bones_indices[i];
        bones_vec_arr[i] = glm::normalize(joints[cur_bone.y] - joints[cur_bone.x]);
    }
    if (!m_is_ar) {
        // the first and fixed point is the root point
        for (int i = 0; i < this->pose_bones_cal_rank.size(); ++i) {
            int cur_bone_index = this->pose_bones_cal_rank[i];
            glm::i32vec2 cur_bone = this->pose_bones_indices[cur_bone_index];
            joints[cur_bone.y] = joints[cur_bone.x] + cur_bones_length[this->pose_bones_length_index[cur_bone_index]] * bones_vec_arr[cur_bone_index];
        }
    }
    else {
        // if is ar mode, I will calculate the raw bone length first
        std::vector<float> raw_bone_length(14, 0);
        for (int i = 0; i < this->pose_bones_indices.size(); ++i) {
            glm::i32vec2 cur_bone = this->pose_bones_indices[i];
            raw_bone_length[i] = glm::length(joints[cur_bone.x] - joints[cur_bone.y]);
        }
        // Adjust the bone length
        raw_bone_length[7] = raw_bone_length[7] * (1 + jitter_range * (this->rand_gen->bounded(2.00001) - 1)); // spin
        raw_bone_length[0] = (0.3829370249427575 * raw_bone_length[7]) * (1 + jitter_range * (this->rand_gen->bounded(2.00001) - 1)); // only change the head now
        raw_bone_length[1] = raw_bone_length[4] = ((raw_bone_length[1] + raw_bone_length[4]) / 2.0) * (1 + jitter_range * (this->rand_gen->bounded(2.00001) - 1));
        raw_bone_length[2] = raw_bone_length[5] = ((raw_bone_length[2] + raw_bone_length[5]) / 2.0) * (1 + jitter_range * (this->rand_gen->bounded(2.00001) - 1));
        raw_bone_length[3] = raw_bone_length[6] = ((raw_bone_length[3] + raw_bone_length[6]) / 2.0) * (1 + jitter_range * (this->rand_gen->bounded(2.00001) - 1));
        raw_bone_length[8] = raw_bone_length[11] = ((raw_bone_length[8] + raw_bone_length[11]) / 2.0) * (1 + jitter_range * (this->rand_gen->bounded(2.00001) - 1));
        raw_bone_length[9] = raw_bone_length[12] = ((raw_bone_length[9] + raw_bone_length[12]) / 2.0) * (1 + jitter_range * (this->rand_gen->bounded(2.00001) - 1));
        raw_bone_length[10] = raw_bone_length[13] = ((raw_bone_length[10] + raw_bone_length[13]) / 2.0) * (1 + jitter_range * (this->rand_gen->bounded(2.00001) - 1));

        for (int i = 0; i < this->pose_bones_cal_rank.size(); ++i) {
            int cur_bone_index = this->pose_bones_cal_rank[i];
            glm::i32vec2 cur_bone = this->pose_bones_indices[cur_bone_index];
            joints[cur_bone.y] = joints[cur_bone.x] + raw_bone_length[cur_bone_index] * bones_vec_arr[cur_bone_index];
        }
    }

    /************************** Test for the optimize method **************************/
//    std::vector<double> opt_bone_length(cur_bones_length.size());
//    std::vector<double> opt_joints(3*joints.size(), 0);
//    glm::vec3 root_joint = joints[joints.size() - 1];
//    glm::vec3 cur_joint;
//    std::copy(cur_bones_length.begin(), cur_bones_length.end(), opt_bone_length.begin());

//    for (int i = 0; i < joints.size(); ++i) {
//        cur_joint = joints[i] - root_joint;
//        opt_joints[3*i + 0] = cur_joint.x; opt_joints[3*i + 1] = cur_joint.y; opt_joints[3*i + 2] = cur_joint.z;
//    }

//    opt_joints = mOurOpt::optimize(opt_joints, opt_bone_length);

//    for (int i = 0; i < joints.size(); ++i) {
//        joints[i] = root_joint + glm::vec3(opt_joints[3*i + 0], opt_joints[3*i + 1], opt_joints[3*i + 2]);
//    }
    /**********************************************************************************/

    /***************************** Show the raw scaled data ***************************/
//    std::vector<float> bones_length;
//    this->calBonesLength(joints, bones_length);
//    glm::vec3 root_pos = joints[14];
//    float scale = 4*9.2f / (bones_length[7] + (bones_length[9] + bones_length[12]) / 2.f);
//    for (int i = 0; i < joints.size(); ++i) {
//        joints[i] = (joints[i] - root_pos) * scale + root_pos;
//    }
    /**********************************************************************************/
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
