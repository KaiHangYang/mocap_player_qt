#ifndef M_POSE_ADJUSTER
#define M_POSE_ADJUSTER

#include <vector>
#include <glm/glm.hpp>
#include <QDebug>
#include <QTime>
#include <QRandomGenerator>

class mPoseAdjuster {
public:
    mPoseAdjuster(const std::vector<float> & bones_length, const std::vector<unsigned int> & bones_length_index, const std::vector<glm::u32vec2> & bones_indices, const std::vector<unsigned int> bones_cal_rank);
    ~mPoseAdjuster();

    void adjustAccordingToBoneLength(std::vector<glm::vec3> &joints, float jitter_range=0);
    void calBonesLength(const std::vector<glm::vec3> & joints, std::vector<float> & bones_length);
    void printBonesLength(const std::vector<float> & bones_length);
    void setBonesLengthJitters(float jitter_size = 0.1);

private:
    std::vector<float> pose_jitter_bones_length;
    std::vector<float> pose_bones_length;
    std::vector<unsigned int> pose_bones_length_index;
    std::vector<glm::u32vec2> pose_bones_indices;
    std::vector<unsigned int> pose_bones_cal_rank;
    QRandomGenerator * rand_gen;
    float jitter_range;
};

#endif
