#ifndef M_POSE_DEFS
#define M_POSE_DEFS
#include <vector>
#include <string>

namespace mPoseDef {
    static const int num_of_joints = 15;
    static const int num_of_bones = 14;
    static const float model_size = 30 * 2;
    static const std::string model_base_dir("/home/kaihang/Projects/QT/mocap_player_qt/models/");
    
    static const std::vector<unsigned int> bone_indices({
            0, 1,
            1, 2,
            2, 3,
            3, 4,
            1, 5,
            5, 6,
            6, 7,
            1, 14,
            14, 8,
            8, 9,
            9, 10,
            14, 11,
            11, 12,
            12, 13,
    });

    static const std::vector<unsigned int> bone_means({
            1, 4,
            2, 5,
            3, 6,
            8, 11,
            9, 12,
            10, 13
            });
}

#endif
