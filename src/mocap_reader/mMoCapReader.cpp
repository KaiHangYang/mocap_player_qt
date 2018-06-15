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

bool mMoCapData::getOneFrame(std::vector<glm::vec3> &joints, std::vector<glm::vec3> &raw_joints, float pose_change_size, float jitter_range, int index) {

    if (this->cur_frame_num >= this->total_frame_num) {
        // The data is not valid
        return false;
    }

    joints = std::vector<glm::vec3>(num_of_joints);
    int frame_index = this->cur_frame_num;

    if (index >= 0 && index < this->total_frame_num) {
        frame_index = index;
        this->getJoints(joints, frame_index);
        raw_joints = joints;
        this->pose_adjuster->adjustAccordingToBoneLength(joints, jitter_range);
        return true;
    }
    else {
        do {
            this->getJoints(joints, frame_index);
            raw_joints = joints;
            this->pose_adjuster->adjustAccordingToBoneLength(joints, jitter_range);
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
        if (dataset != 0 && dataset != 1) {
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

        std::vector<glm::vec3> neck_up_joints = bvh_joints[this->statistic_joints_arrs[dataset][0]]->pos();
        std::vector<glm::vec3> neck_down_joints = bvh_joints[this->statistic_joints_arrs[dataset][1]]->pos();
        std::vector<glm::vec3> spin_joints = bvh_joints[this->statistic_joints_arrs[dataset][2]]->pos();

        /********** Here I can statistic the ratio of up neck and lower neck ***********/
//        QFile file("/home/kaihang/Desktop/statistic.txt");
//        file.open(QIODevice::Append);
//        QTextStream file_stream(&file);

//        for (int i = 0; i < neck_up_joints.size(); ++i) {
//            float length_1 = glm::length(neck_up_joints[i] - spin_joints[i]);
//            float length_2 = glm::length(neck_down_joints[i] - spin_joints[i]);
//            file_stream << length_2 / length_1 << " ";
//        }
//        file.close();
        /********************************************************************************/
    }
    else if (surfix == "mpi") {
        if (dataset != 2) {
            qDebug() << "Only dataset 2 accept .mpi";
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
    else if (surfix == "h36") {
        if (dataset != 3) {
            qDebug() << "Only dataset 3 accept .h36";
            return false;
        }
        QFile data_file(file_path);
        if (data_file.open(QIODevice::ReadOnly)) {
            QByteArray data = data_file.readAll();
            std::vector<unsigned int> cur_valid_joints_arr = this->valid_joints_arrs[dataset];
            char * data_ptr = data.data();
            total_frame_nums = *((int *)(data_ptr));
            num_of_joints = cur_valid_joints_arr.size();

            data_ptr += 4; // point to the start of the joints_3d
            float * joints_3d = (float *)data_ptr;
            frame_datas = std::vector<std::vector<glm::vec3>>(num_of_joints);
            for (int i = 0; i < num_of_joints; ++i) {
                frame_datas[i] = std::vector<glm::vec3>(total_frame_nums);
                for (int j = 0; j < total_frame_nums; ++j) {
                    int initial_index = num_of_joints * 3 * j + 3 * i;
                    frame_datas[i][j] = glm::vec3(joints_3d[initial_index + 0], joints_3d[initial_index + 1], joints_3d[initial_index + 2]);
                }
            }
            // Adjust the neck joints for h36 dataset
            for (int frame_num = 0; frame_num < total_frame_nums; ++frame_num) {
                glm::vec3 neck_point = frame_datas[1][frame_num];
                glm::vec3 root_point = frame_datas[14][frame_num];
                // statistic average 0.71540362165092952
                glm::vec3 dir = glm::normalize(neck_point - root_point);
                neck_point = 0.79540362165092952f * glm::length(neck_point - root_point) * dir + root_point;
                frame_datas[1][frame_num] = neck_point;
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
