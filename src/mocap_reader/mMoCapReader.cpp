#include "mMoCapReader.h"
#include <QFileInfo>
#include <QDebug>

bool mMoCapData::getJoints(std::vector<glm::vec3> &joints, int frame_index) {
    for (int i = 0; i < this->num_of_joints; ++i) {
        joints[i] = this->data[i][frame_index];
    }
    // adjust the point for different dataset
    if (this->cur_dataset_num == 1) {
        joints[14] = (joints[11] + joints[8]) * 0.5f;
    }
}

bool mMoCapData::getOneFrame(std::vector<glm::vec3> &joints, float pose_change_size, int index) {

    if (this->cur_frame_num >= this->total_frame_num) {
        // The data is not valid
        return false;
    }

    joints = std::vector<glm::vec3>(num_of_joints);
    int frame_index = this->cur_frame_num;
    std::vector<glm::vec3> scaled_joints;

    if (index >= 0 && index < this->total_frame_num) {
        frame_index = index;
        this->getJoints(joints, frame_index);
        this->pose_adjuster->adjustAccordingToBoneLength(joints, this->is_use_jitters);
        return true;
    }
    else {
        do {
            this->getJoints(joints, frame_index);
            this->pose_adjuster->adjustAccordingToBoneLength(joints, this->is_use_jitters);
            this->cur_frame_num++;
            frame_index = this->cur_frame_num;
        } while (this->calMaxChange(this->prev_choosed_data, joints) < pose_change_size && this->cur_frame_num < this->total_frame_num);

        if (this->cur_frame_num > this->total_frame_num) {
            return false;
        }

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

float mMoCapData::calMaxChange(std::vector<glm::vec3> prev, std::vector<glm::vec3> cur) {
    mPoseDef::scalePose(prev);
    mPoseDef::scalePose(cur);

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
        if (dataset != 0 || dataset != 1) {
            qDebug() << "Only dataset 0 and 1 accept bvh!";
            return false;
        }

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
    else if (surfix == "mpi") {
        if (dataset != 2) {
            qDebug() << "Only data 2 accept mpi";
            return false;
        }
        QFile data_file(file_path);
        if (data_file.open(QIODevice::ReadOnly)) {
            QByteArray data = data_file.readAll();
            std::vector<unsigned int> cur_valid_joints_arr = this->valid_joints_arrs[dataset];
            char * data_ptr = data.data();
            total_frame_nums = *((int *)(data_ptr));
            num_of_joints = cur_valid_joints_arr.size();

            data_ptr += 4 * (1 + 2 * num_of_joints * total_frame_nums); // point to the start of the joints_3d
            float * joints_3d = (float *)data_ptr;
            frame_datas = std::vector<std::vector<glm::vec3>>(num_of_joints);
            for (int i = 0; i < num_of_joints; ++i) {
                frame_datas[i] = std::vector<glm::vec3>(total_frame_nums);
                for (int j = 0; j < total_frame_nums; ++j) {
                    int initial_index = num_of_joints * 3 * j + 3 * i;
                    frame_datas[i][j] = glm::vec3(joints_3d[initial_index + 0], joints_3d[initial_index + 1], joints_3d[initial_index + 2]);
                }
            }
        }
        else {
            qDebug() << "mMoCapReader.cpp: failed to read " << file_path;
            return false;
        }
    }
    else {
        qDebug() << "mMoCapReader.cpp: Mocap data file " << file_path << " is not support!";
        return false;
    }

    data->setData(frame_datas, total_frame_nums, num_of_joints, dataset);
    return true;
}
