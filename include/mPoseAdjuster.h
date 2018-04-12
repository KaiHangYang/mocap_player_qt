#ifndef M_POSE_ADJUSTER
#define M_POSE_ADJUSTER

#include <vector>
#include <glm/glm.hpp>
#include <QDebug>

class mPoseAdjuster {
public:
    mPoseAdjuster(const std::vector<float> & bones_length, const std::vector<unsigned int> & bones_length_index, const std::vector<glm::u32vec2> & bones_indices, const std::vector<unsigned int> bones_cal_rank): pose_bones_length(bones_length), pose_bones_indices(bones_indices), pose_bones_cal_rank(bones_cal_rank), pose_bones_length_index(bones_length_index){}
    ~mPoseAdjuster(){}

    void adjustAccordingToBoneLength(std::vector<glm::vec3> &joints, bool with_jitter=false, float jitter_size=0.1);
    void calBonesLength(const std::vector<glm::vec3> & joints, std::vector<float> & bones_length);
    void printBonesLength(const std::vector<float> & bones_length);
private:
    std::vector<float> pose_bones_length;
    std::vector<float> pose_bones_length_index;
    std::vector<glm::u32vec2> pose_bones_indices;
    std::vector<unsigned int> pose_bones_cal_rank;
};

#endif
