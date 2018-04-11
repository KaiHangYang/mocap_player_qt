#ifndef M_MOCAP_READER
#define M_MOCAP_READER

#include "bvh-parser/bvh-parser.h"
#include <QString>
#include <glm/glm.hpp>
#include <vector>

class mMoCapData {
public:
    mMoCapData():total_frame_num(0), cur_frame_num(0), num_of_joints(0) {}
    void setData(std::vector<std::vector<glm::vec3>> data, int total_frame_num, int num_of_joints) {
        this->data = data;
        this->cur_frame_num = 0;
        this->total_frame_num = total_frame_num;
        this->num_of_joints = num_of_joints;
    }

    mMoCapData(std::vector<std::vector<glm::vec3>> data, int total_frame_num, int num_of_joints) : data(data), cur_frame_num(0), total_frame_num(total_frame_num), num_of_joints(num_of_joints) {}
    ~mMoCapData(){}
    bool getOneFrame(std::vector<glm::vec3> & joints, int index=-1);
    bool getOneFrame(std::vector<float> & joints, glm::mat4 cam_ex_mat, int index=-1);
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
