#include "mMoCapReader.h"
#include <QFileInfo>
#include <QDebug>

bool mMoCapData::getOneFrame(std::vector<glm::vec3> &joints, float pose_change_size, int index) {

    if (this->cur_frame_num >= this->total_frame_num) {
        // The data is not valid
        return false;
    }

    joints = std::vector<glm::vec3>(num_of_joints);
    int frame_index = this->cur_frame_num;

    if (index >= 0 && index < this->total_frame_num) {
        frame_index = index;
        for (int i = 0; i < this->num_of_joints; ++i) {
            joints[i] = this->data[i][frame_index];
        }

        this->pose_adjuster->adjustAccordingToBoneLength(joints, this->is_use_jitters);
        return true;
    }
    else {
        do {
            for (int i = 0; i < this->num_of_joints; ++i) {
                joints[i] = this->data[i][frame_index];
            }
            this->cur_frame_num++;
            frame_index = this->cur_frame_num;
        } while (this->calMaxChange(this->prev_choosed_data, joints) < pose_change_size && this->cur_frame_num < this->total_frame_num);

        if (this->cur_frame_num > this->total_frame_num) {
            return false;
        }

        this->pose_adjuster->adjustAccordingToBoneLength(joints, this->is_use_jitters);
        this->prev_choosed_data = joints;
        return true;
    }
}
//bool mMoCapData::getOneFrame(std::vector<glm::vec3> & joints, glm::mat4 cam_ex_mat, float pose_change_size, int index) {
//    std::vector<glm::vec3> cur_joints;

//    if (this->getOneFrame(cur_joints, pose_change_size, index)) {
//        joints.clear();
//        for (int i = 0; i < cur_joints.size(); ++i) {
//            glm::vec4 cur_pos = cam_ex_mat * glm::vec4(cur_joints[i],1.0);
//            joints.push_back(glm::vec3(cur_pos));
//        }
//        return true;
//    }
//    else {
//        return false;
//    }
//}

float mMoCapData::calMaxChange(const std::vector<glm::vec3> & prev, const std::vector<glm::vec3> & cur) {
    float max_change = 0.0f;
    float cur_point_change = 0;
    for (int i = 0; i < prev.size(); ++i) {
        cur_point_change = glm::length(prev[i] - cur[i]);
        if (cur_point_change >= max_change) {
            max_change = cur_point_change;
        }
    }
    return max_change;
}

void mMoCapData::resetCounter() {
    this->cur_frame_num = 0;
}

void mMoCapData::setFramePos(int frame_num) {
    frame_num -= 1;
    frame_num = (frame_num < 0)?0:((frame_num >= this->total_frame_num)?frame_num-1:frame_num);
    this->cur_frame_num = frame_num;
}

int mMoCapData::getTotalFrame() {
    return this->total_frame_num;
}
int mMoCapData::getCurFrame() {
    return this->cur_frame_num + 1;
}
void mMoCapData::clear() {
    this->cur_frame_num = 0;
    this->total_frame_num = 0;
    this->num_of_joints = 0;
    this->data.clear();
}

bool mMoCapReader::parse(QString file_path, int dataset, mMoCapData * data) {
    QFileInfo file_info(file_path);

    if (!file_info.isFile()) {
        qDebug() << "mMoCapReader.cpp: " << file_path  << " is not existing!";
        return false;
    }

    if (dataset < 0 || dataset >= this->valid_joints_arrs.size()) {
        qDebug() << "mMoCapReader.cpp: " << "Dataset num " << dataset << " is not supported!";
        return false;
    }

    QString surfix = file_path.split(".").back();
    std::vector<std::vector<glm::vec3>> frame_datas;
    int total_frame_nums = 0;
    int num_of_joints = 0;

    if (surfix == "bvh") {
        std::vector<unsigned int> cur_valid_joints_arr = this->valid_joints_arrs[dataset];
        bvh::Bvh bvh_content;
        bvh::Bvh_parser bvh_parser;

        bvh_parser.parse(file_path.toStdString(), &bvh_content);
        bvh_content.recalculate_joints_ltm(bvh_content.root_joint());
        total_frame_nums = bvh_content.num_frames();
        num_of_joints = cur_valid_joints_arr.size();
        frame_datas = std::vector<std::vector<glm::vec3>>(num_of_joints);
        std::vector<std::shared_ptr<bvh::Joint>> bvh_joints = bvh_content.joints();

        for (int i = 0; i < num_of_joints; ++i) {
            frame_datas[i] = bvh_joints[cur_valid_joints_arr[i]]->pos();
        }

    }
    else {
        qDebug() << "mMoCapReader.cpp: Mocap data file " << file_path << " is not support!";
        return false;
    }

    data->setData(frame_datas, total_frame_nums, num_of_joints);
    return true;
}
