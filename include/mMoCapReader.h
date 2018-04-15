#ifndef M_MOCAP_READER
#define M_MOCAP_READER

#include "bvh-parser/bvh-parser.h"
#include <QString>
#include <glm/glm.hpp>
#include <vector>
#include "mPoseAdjuster.h"
#include "mPoseDefs.h"

class mMoCapData {
public:
    mMoCapData():total_frame_num(0), cur_frame_num(0), num_of_joints(0) {}
    void setData(std::vector<std::vector<glm::vec3>> data, int total_frame_num, int num_of_joints) {
        this->data = data;
        this->cur_frame_num = 0;
        this->total_frame_num = total_frame_num;
        this->num_of_joints = num_of_joints;
        this->prev_choosed_data = std::vector<glm::vec3>(this->num_of_joints, glm::vec3(0.f));
        this->is_use_jitters = false;
        this->pose_adjuster = new mPoseAdjuster(mPoseDef::bones_length, mPoseDef::bones_length_index, mPoseDef::bones_indices,mPoseDef::bones_cal_rank);
    }

    mMoCapData(std::vector<std::vector<glm::vec3>> data, int total_frame_num, int num_of_joints) : data(data), cur_frame_num(0), total_frame_num(total_frame_num), num_of_joints(num_of_joints) {
        this->is_use_jitters = false;
        this->pose_adjuster = new mPoseAdjuster(mPoseDef::bones_length, mPoseDef::bones_length_index, mPoseDef::bones_indices,mPoseDef::bones_cal_rank);
    }
    ~mMoCapData(){}
    bool getOneFrame(std::vector<glm::vec3> & joints, float pose_change_size=0.f, int index=-1);
//    bool getOneFrame(std::vector<glm::vec3> & joints, glm::mat4 cam_ex_mat, float pose_change_size=0.f, int index=-1);
    void resetCounter();
    void setFramePos(int frame_num);
    int getTotalFrame();
    int getCurFrame();
    void clear();

private:
    int cur_frame_num;
    int total_frame_num;
    int num_of_joints;
    std::vector<std::vector<glm::vec3>> data;
    std::vector<glm::vec3> prev_choosed_data;
    mPoseAdjuster * pose_adjuster;
    float calMaxChange(std::vector<glm::vec3> prev, std::vector<glm::vec3> cur);
    bool is_use_jitters;
};

class mMoCapReader {
public:
    mMoCapReader() {
        this->valid_joints_arrs = std::vector<std::vector<unsigned int>>({
                                                                             {14, 13, 24, 25, 26, 17, 18, 19, 6, 7, 8, 1, 2, 3, 0} // sfu dataset
                                                                         });
    }
    ~mMoCapReader(){}
    // dataset represent the dataset to parse
    // 0 is the sfu data, 1 is the cmu data...
    bool parse(QString file_path, int dataset, mMoCapData * data);
private:
    std::vector<std::vector<unsigned int>> valid_joints_arrs;

};

#endif
