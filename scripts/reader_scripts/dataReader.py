import numpy as np
import os
import sys

def img_file_cmp(a, b):
    frame_num_a = int(os.path.basename(a).split("-")[0])
    frame_num_b = int(os.path.basename(b).split("-")[0])

    camera_num_a = int(os.path.basename(a).split(".")[0].split("-")[1])
    camera_num_b = int(os.path.basename(b).split(".")[0].split("-")[1])

    if frame_num_a > frame_num_b:
        return 1
    elif frame_num_a < frame_num_b:
        return -1
    else:
        if (camera_num_a > camera_num_b):
            return 1
        else:
            return -1

def label_file_cmp(a, b):
    frame_num_a = int(os.path.basename(a).split(".")[0].split("-")[0])
    frame_num_b = int(os.path.basename(b).split(".")[0].split("-")[0])

    if frame_num_a > frame_num_b:
        return 1
    else:
        return -1

class mDataReader:
    def __init__(self, is_ar=False):
        self.is_ar = is_ar
        self.init_data()

    def init_data(self, camera_num = 0, data_file_name = ""):
        self.camera_num = camera_num
        self.data_file_name = data_file_name
        self.data_frames = []
        self.data_labels = []
        self.data_labels_raw = []
        self.cur_frame_index = 0
        self.total_frame_num = 0

    def reset(self):
        self.cur_frame_index = 0

    def validCamera(self, camera_num):
        if camera_num >= 0 and camera_num < self.camera_num:
            return True
        else:
            return False

    def getOneData(self, index=-1, camera_num=0):
        if camera_num < 0 or camera_num >= self.camera_num:
            return False, None, None, None
        if index >= 0 and index < self.total_frame_num:
            if self.is_ar:
                return True, self.data_frames[index][camera_num], self.data_labels[index][camera_num], self.data_labels_raw[index][camera_num]
            else:
                return True, self.data_frames[index][camera_num], self.data_labels[index][camera_num], None
        else:
            index = self.cur_frame_index
            if index >= self.total_frame_num:
                return False, None, None, None

            self.cur_frame_index += 1
            if self.is_ar:
                return True, self.data_frames[index][camera_num], self.data_labels[index][camera_num], self.data_labels_raw[index][camera_num]
            else:
                return True, self.data_frames[index][camera_num], self.data_labels[index][camera_num], None

    def parse(self, data_path, camera_num):
        self.init_data(camera_num, data_path)

        imgs_list = []
        labels_list = []
        labels_raw_list = []

        if os.path.isdir(data_path):
            dir_list = os.listdir(data_path)

            for item in dir_list:
                if item.split(".")[1] == "txt":
                    if len(item.split(".")[0].split("-")) > 1 and item.split(".")[0].split("-")[1] == "raw":
                        labels_raw_list.append(os.path.join(self.data_file_name, item))
                    else:
                        labels_list.append(os.path.join(self.data_file_name, item))
                else:
                    imgs_list.append(os.path.join(self.data_file_name, item))
            imgs_list.sort(img_file_cmp)
            self.data_frames = np.reshape(imgs_list, [-1, self.camera_num]).tolist()

            labels_list.sort(label_file_cmp)
            labels_raw_list.sort(label_file_cmp)

            self.data_labels = [self.parseLables(i) for i in labels_list]
            self.data_labels_raw = [self.parseLables(i) for i in labels_raw_list]

            self.total_frame_num = len(self.data_labels)

    def parseLables(self, data_file):
        with open(data_file) as f:
            datas = f.readlines()
            frame_labels = []
            for i in range(len(datas) / 3):
                labels_2d = np.reshape(datas[3*i + 1].strip().split(" "), [-1, 2]).astype(np.float32)
                labels_3d = np.reshape(datas[3*i + 2].strip().split(" "), [-1, 3]).astype(np.float32)

                frame_labels.append([labels_2d, labels_3d])

            return frame_labels


if __name__ == "__main__":
    reader = mDataReader()
    reader.parse("/home/kaihang/Desktop/test_dir/0005_SideSkip001", 6)
    print(len(reader.data_frames))
    print(len(reader.data_labels))
