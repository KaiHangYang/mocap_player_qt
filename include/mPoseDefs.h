#ifndef M_POSE_DEFS
#define M_POSE_DEFS
#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace mPoseDef {
    static const int num_of_joints = 15;
    static const int num_of_bones = 14;
    static const int root_of_joints = 14;
    static const float model_size = 30 * 2;
    static const float model_joint_bone_ratio = 1.3;

    static const std::string model_base_dir("/home/kaihang/Projects/QT/mocap_player_qt/models/");
//    static const std::string model_base_dir("/Users/kaihang/Desktop/Code/Cpp/Qt/mocap_player_qt/models/");

    static const int spine_bone_index = 7;
    static const int left_hip_index = 8;
    static const int right_hip_index = 11;
    
    static const std::vector<glm::u32vec2> bones_indices({
            glm::u32vec2(1, 0), // 0
            glm::u32vec2(1, 2), // 1
            glm::u32vec2(2, 3), // 2
            glm::u32vec2(3, 4), // 3
            glm::u32vec2(1, 5), // 4
            glm::u32vec2(5, 6), // 5
            glm::u32vec2(6, 7), // 6
            glm::u32vec2(14, 1),// 7
            glm::u32vec2(14, 8),// 8
            glm::u32vec2(8, 9), // 9
            glm::u32vec2(9, 10),// 10
            glm::u32vec2(14, 11),//11
            glm::u32vec2(11, 12),//12
            glm::u32vec2(12, 13)//13
    });
    static const std::vector<unsigned int> bones_cal_rank({
        7, 8, 11, 9, 10, 12, 13, 0, 1, 4, 2, 3, 5, 6
    });
    static const std::vector<float> bones_length({6.90715, 6.71018, 10.9442, 8.52009, 18.0373, 3.64953, 15.7058, 15.4187});
    static const std::vector<double> bones_length_dbl({6.90715, 6.71018, 10.9442, 8.52009, 18.0373, 3.64953, 15.7058, 15.4187});
    static const std::vector<unsigned int> bones_length_index({0, 1, 2, 3, 1, 2, 3, 4, 5, 6, 7, 5, 6, 7});


    /******** Notice: The joints must be root related ********/
    static float scaleTargetLength = 920;
    static void scalePose(std::vector<glm::vec3> & joints) {
        float target_length = scaleTargetLength;
        // 7, 9, 10, 12, 13
        float cur_length = glm::length(joints[bones_indices[7].x] - joints[bones_indices[7].y]);
        cur_length += 0.5 * (glm::length(joints[bones_indices[9].x] - joints[bones_indices[9].y]));
        cur_length += 0.5 * (glm::length(joints[bones_indices[12].x] - joints[bones_indices[12].y]));

        float scale = target_length / cur_length;
        for (int i = 0; i < joints.size(); ++i) {
            joints[i] *= scale;
        }
    }
}

#endif
