import numpy as np
import scipy.io as sio
import os
import struct


##### Currently I only get the points of camera 8 #####

class MPITransfer():
    def __init__(self):
        # Currently use the lower neck point as neck point
        self.valid_keypoint_arr = [7, 1, 14, 15, 16, 9, 10, 11, 23, 24, 25, 18, 19, 20, 4]
        self.cam_ex_mat_inv = np.linalg.inv(np.reshape([0.000575281, 0.06160985, -0.9981001, 221.3543, 0.2082146, -0.9762325, -0.06013997, 659.87, -0.978083, -0.2077844, -0.01338968, 3644.688, 0, 0, 0, 1], [4, 4]))
        self.camera_num = 8
        self.raw_joint_num = 28
        self.cur_joint_num = 15

    def parse(self, mat_path, save_path):
        if not os.path.isfile(mat_path):
            print("Mat path is not valid!")
            quit()

        mat_data = sio.loadmat(mat_path)
        raw_joints_2d = np.reshape(mat_data["annot2"][self.camera_num, 0], [-1, self.raw_joint_num, 2]).astype(np.float32)[:, self.valid_keypoint_arr]
        raw_joints_3d = np.reshape(mat_data["univ_annot3"][self.camera_num, 0], [-1, self.raw_joint_num, 3]).astype(np.float32)[:, self.valid_keypoint_arr]

        for i in range(raw_joints_3d.shape[0]):
            for j in range(raw_joints_3d.shape[1]):
                raw_joints_3d[i, j] = np.dot(self.cam_ex_mat_inv, np.concatenate([raw_joints_3d[i, j], [1.0]], 0))[0:3]

        root_points = raw_joints_3d[:, 14].copy()
        for i in range(raw_joints_3d.shape[1]):
            raw_joints_3d[:, i, :] -= root_points

        with open(save_path, "wb") as f:
            frame_num = raw_joints_2d.shape[0]

            joints_2d = raw_joints_2d.flatten().tolist()
            joints_3d = raw_joints_3d.flatten().tolist()
            f.write(struct.pack("i", frame_num))
            f.write(struct.pack("%df" % len(joints_2d), *joints_2d))
            f.write(struct.pack("%df" % len(joints_3d), *joints_3d))

        saved_joints2d, saved_joints3d = self.read(save_path)
        assert((saved_joints2d == raw_joints_2d).all() and (saved_joints3d == raw_joints_3d).all())

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

        return np.reshape(joints_2d, [-1, self.cur_joint_num, 2]), np.reshape(joints_3d, [-1, self.cur_joint_num, 3])


if __name__ == "__main__":
    transfer = MPITransfer()
    OBJ_LIST = [1, 2, 3, 4 ,5 ,6 ,7, 8]
    SEQ_LIST = [1, 2]

    for obj_num in OBJ_LIST:
        for seq_num in SEQ_LIST:
            transfer.parse("/home/kaihang/DataSet/3DPoseDataSets/3D/MPI_INF_3DHP/S%d/Seq%d/annot.mat" % (obj_num, seq_num), "/home/kaihang/DataSet/MoCap/mpi/o%d-s%d.mpi" % (obj_num, seq_num))
