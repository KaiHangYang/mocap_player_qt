import numpy as np
import scipy.io as sio
import os
import struct


##### Currently I only get the points of camera 8 #####

# 4 is the unive 3d points, 3 is the normal 3d points
type_3d_points = 3

class MPISelectedTransfer():
    def __init__(self):
        # Currently use the lower neck point as neck point
        self.cam_ex_mat_invs = []

        self.cam_ex_mat_invs.append(np.linalg.inv(np.reshape([0.9650164, 0.00488022, 0.262144, -562.8666, -0.004488356, -0.9993728, 0.0351275, 1398.138, 0.262151, -0.03507521, -0.9643893, 3852.623, 0, 0, 0, 1], [4, 4])))
        self.cam_ex_mat_invs.append(np.linalg.inv(np.reshape([0.6050639, -0.02184232, 0.7958773, -1429.856, -0.22647, -0.9630526, 0.1457429, 738.1779, 0.7632883, -0.2684261, -0.587655, 4897.966, 0, 0, 0, 1], [4, 4])))
        self.cam_ex_mat_invs.append(np.linalg.inv(np.reshape([-0.3608179, -0.009492658, 0.932588, 57.25702, -0.0585942, -0.9977421, -0.03282591, 1307.287, 0.9307939, -0.06648842, 0.359447, 2799.822, 0, 0, 0, 1], [4, 4])))
        self.cam_ex_mat_invs.append(np.linalg.inv(np.reshape([-0.0721105, -0.04817664, 0.9962325, -284.8168, -0.4393254, -0.8951841, -0.07508985, 807.9184, 0.895429, -0.443085, 0.04338695, 3177.16, 0, 0, 0, 1], [4, 4])))
        self.cam_ex_mat_invs.append(np.linalg.inv(np.reshape([0.3737275, 0.09688602, 0.9224646, -1563.911, -0.009716132, -0.9940662, 0.1083427, 801.9608, 0.9274878, -0.04945343, -0.3705685, 3517.316, 0, 0, 0, 1], [4, 4])))
        self.cam_ex_mat_invs.append(np.linalg.inv(np.reshape([-0.9995936, 0.02847456, 0.001368653, 1378.866, -0.02843213, -0.9992908, 0.0246889, 1270.781, 0.002070688, 0.02463995, 0.9996943, 2631.567, 0, 0, 0, 1], [4, 4])))
        self.cam_ex_mat_invs.append(np.linalg.inv(np.reshape([0.000575281, 0.06160985, -0.9981001, 221.3543, 0.2082146, -0.9762325, -0.06013997, 659.87, -0.978083, -0.2077844, -0.01338968, 3644.688, 0, 0, 0, 1], [4, 4])))
        self.cam_ex_mat_invs.append(np.linalg.inv(np.reshape([0.04176839, 0.00780962, -0.9990969, 388.6217, 0.5555364, -0.831324, 0.01672664, 137.5452, -0.8304425, -0.5557333, -0.03906159, 4216.635, 0, 0, 0, 1], [4, 4])))

        self.camera_nums = [0, 1, 2, 3, 4, 7, 8, 9]
        self.cur_joint_num = 15

    def parse(self, npy_path, save_dir):
        if not os.path.isfile(npy_path):
            print("npy path is not valid!")
            quit()

        if not os.path.isdir(save_dir):
            print("The directory for saving is not existing!")
            quit()

        cur_datas = np.load(npy_path).tolist()

        for camera_num_index in range(len(self.camera_nums)):
            camera_num = self.camera_nums[camera_num_index]

            save_path_train = os.path.join(save_dir, "%d-train.mpi" % camera_num)
            save_path_valid = os.path.join(save_dir, "%d-valid.mpi" % camera_num)

            cur_video_name = "video%d" % camera_num
            cur_video_labels = cur_datas[cur_video_name]

            cur_labels_3d_train = []
            cur_labels_2d_train = []

            cur_labels_3d_valid = []
            cur_labels_2d_valid = []

            # Now only save the joints_2d and the joints_3d
            for cur_label in cur_video_labels:
                tmp_label_2d = cur_label[2].copy()
                tmp_label_3d = []

                for cur_point_index in range(cur_label[2].shape[0]):
                    tmp_label_3d.append(np.dot(self.cam_ex_mat_invs[camera_num_index], np.concatenate([cur_label[type_3d_points][cur_point_index], [1.0]], 0))[0:3])

                if cur_label[0][3]:
                    cur_labels_2d_train.append(tmp_label_2d)
                    cur_labels_3d_train.append(np.array(tmp_label_3d))
                else:
                    cur_labels_2d_valid.append(tmp_label_2d)
                    cur_labels_3d_valid.append(np.array(tmp_label_3d))

            cur_labels_3d_train = np.array(cur_labels_3d_train, dtype=np.float32)
            cur_labels_2d_train = np.array(cur_labels_2d_train, dtype=np.float32)

            cur_labels_3d_valid = np.array(cur_labels_3d_valid, dtype=np.float32)
            cur_labels_2d_valid = np.array(cur_labels_2d_valid, dtype=np.float32)

            f_train = open(save_path_train, "wb")
            f_valid = open(save_path_valid, "wb")


            # Save the training data
            total_frame_sum_train = cur_labels_3d_train.shape[0]

            joints_2d_train = cur_labels_2d_train.flatten().tolist()
            joints_3d_train = cur_labels_3d_train.flatten().tolist()
            f_train.write(struct.pack("i", total_frame_sum_train))
            f_train.write(struct.pack("%df" % len(joints_2d_train), *joints_2d_train))
            f_train.write(struct.pack("%df" % len(joints_3d_train), *joints_3d_train))

            # Save the valid data
            total_frame_sum_valid = cur_labels_3d_valid.shape[0]

            joints_2d_valid = cur_labels_2d_valid.flatten().tolist()
            joints_3d_valid = cur_labels_3d_valid.flatten().tolist()
            f_valid.write(struct.pack("i", total_frame_sum_valid))
            f_valid.write(struct.pack("%df" % len(joints_2d_valid), *joints_2d_valid))
            f_valid.write(struct.pack("%df" % len(joints_3d_valid), *joints_3d_valid))

            f_train.close()
            f_valid.close()

            # Check the train data
            saved_joints2d, saved_joints3d = self.read(save_path_train)
            assert((saved_joints2d == cur_labels_2d_train).all() and (saved_joints3d == cur_labels_3d_train).all())

            # Check the valid data
            saved_joints2d, saved_joints3d = self.read(save_path_valid)
            assert((saved_joints2d == cur_labels_2d_valid).all() and (saved_joints3d == cur_labels_3d_valid).all())

    def read(self, data_path):
        with open(data_path, "rb") as f:
            data = f.read()
            frame_num = struct.unpack("i", data[0:4])[0]
            cur_index = 4
            cur_num = frame_num * 2 * self.cur_joint_num
            joints_2d = struct.unpack("%df" % (cur_num), data[cur_index: cur_index + cur_num * 4])
            cur_index += cur_num * 4
            cur_num = frame_num * 3 * self.cur_joint_num
            joints_3d = struct.unpack("%df" % (cur_num), data[cur_index: cur_index + cur_num * 4])

        return np.reshape(joints_2d, [-1, self.cur_joint_num, 2]).astype(np.float32), np.reshape(joints_3d, [-1, self.cur_joint_num, 3]).astype(np.float32)


if __name__ == "__main__":
    transfer = MPISelectedTransfer()
    transfer.parse("/home/kaihang/DataSet/cropped_mpi_data/combined.npy", "/home/kaihang/Desktop/test_dir")
    # data_2d, data_3d = transfer.read("/home/kaihang/Desktop/test_dir/3.mpi")
