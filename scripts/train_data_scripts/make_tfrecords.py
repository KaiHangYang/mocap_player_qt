import numpy as np
import cv2
import os
import tensorflow as tf
import sys

sys.path.append("../")

from reader_scripts import dataReader as data_reader
from visual_tools import display_utils

pad_scale = 0.2
target_image_size = 368

def get_crop(img, points, num_of_joints):
    is_discard = False

    img_width = img.shape[1]
    img_height = img.shape[0]

    points = points[(points > np.array([0, 0])).all(axis=1)]

    max_x = np.max(points[:, 0])
    max_y = np.max(points[:, 1])
    min_x = np.min(points[:, 0])
    min_y = np.min(points[:, 1])

    cen_x = (max_x + min_x) / 2
    cen_y = (max_y + min_y) / 2

    crop_img_size = int(max(max_x - min_x, max_y - min_y) * (1 + pad_scale))

    min_x = int(cen_x - crop_img_size / 2)
    min_y = int(cen_y - crop_img_size / 2)
    max_x = min_x + crop_img_size
    max_y = min_y + crop_img_size

    raw_min_x = 0
    raw_max_x = img_width
    raw_min_y = 0
    raw_max_y = img_height

    if min_x > 0:
        raw_min_x = min_x
    if min_y > 0:
        raw_min_y = min_y
    if max_x < img_width:
        raw_max_x = max_x
    if max_y < img_height:
        raw_max_y = max_y

    cropped_img = cv2.copyMakeBorder(img[raw_min_y:raw_max_y, raw_min_x:raw_max_x], top=raw_min_y-min_y, bottom=max_y - raw_max_y, left=raw_min_x - min_x, right=max_x - raw_max_x, value=[128, 128, 128], borderType=cv2.BORDER_CONSTANT)
    if cropped_img is None or cropped_img.shape[0] != cropped_img.shape[1]:
        is_discard = True
    return cropped_img, (min_x, min_y, max_x, max_y), is_discard


def data_resize_with_cropped(img, joints2d, num_of_joints=15):

    points = np.reshape(joints2d, (num_of_joints, 2))

    img_cropped, offset_cropped, is_discard = get_crop(img, joints2d, num_of_joints)

    if is_discard:
        return img, points, offset_cropped, 1.0, is_discard

    for i in range(points.shape[0]):
        if not (points[i, 0] == 0 and points[i, 1] == 0):
            points[i, 0] = points[i, 0] - offset_cropped[0]
            points[i, 1] = points[i, 1] - offset_cropped[1]

    scale = float(img_cropped.shape[0]) / target_image_size

    img = cv2.resize(img_cropped, (target_image_size, target_image_size))

    points = np.float32(points)
    points /= scale

    return img, points, offset_cropped, scale, False


if __name__ == "__main__":

    # Then write the tfrecords
    is_ar = False

    if is_ar:
        camera_num = 1
    else:
        camera_num = 36

    # train_data_path = "/home/kaihang/DataSet_2/MocapData/mpi_mocap/raw_data/train"
    # test_data_path = "/home/kaihang/DataSet_2/MocapData/mpi_mocap/raw_data/valid"

    # train_data_path = "/home/kaihang/DataSet_2/MocapData/cmu_mocap/raw_datas/train/"
    # test_data_path = "/home/kaihang/DataSet_2/MocapData/cmu_mocap/raw_datas/valid/"

    # train_writer = tf.python_io.TFRecordWriter("/home/kaihang/DataSet_2/MocapData/cmu_mocap/tfrecords/sfu_train.tfrecord")
    # test_writer = tf.python_io.TFRecordWriter("/home/kaihang/DataSet_2/MocapData/cmu_mocap/tfrecords/sfu_valid.tfrecord")

    train_data_path = "/home/kaihang/Desktop/test_dir/cmu_data/render_type_2/train"
    test_data_path = "/home/kaihang/Desktop/test_dir/cmu_data/render_type_2/valid"

    train_writer = tf.python_io.TFRecordWriter("/home/kaihang/DataSet_2/MocapData/cmu_mocap/tfrecords/cmu_train_render_2.tfrecord")
    test_writer = tf.python_io.TFRecordWriter("/home/kaihang/DataSet_2/MocapData/cmu_mocap/tfrecords/cmu_valid_render_2.tfrecord")

    train_dataset_list = os.listdir(train_data_path)
    valid_dataset_list = os.listdir(test_data_path)

    train_dataset_list.sort()
    valid_dataset_list.sort()

    print(train_dataset_list)
    print(valid_dataset_list)

    train_dir_lists = [os.path.join(train_data_path, i) for i in train_dataset_list]
    test_dir_lists = [os.path.join(test_data_path, i) for i in valid_dataset_list]

    reader = data_reader.mDataReader(is_ar)

    # generate train data
    train_data_sum = 0
    for train_file in train_dir_lists:
        reader.parse(train_file, camera_num)

        for cur_camera in range(camera_num):
            reader.reset()
            while reader.cur_frame_index < reader.total_frame_num:
                is_valid, img_path, labels, labels_raw = reader.getOneData(camera_num = cur_camera)
                img = cv2.imread(img_path)

                if is_ar:
                    labels_2d_raw = labels_raw[0].copy()
                else:
                    labels_2d_raw = labels[0].copy()

                labels_2d = labels[0].copy()
                labels_3d = labels[1].copy()

                # labels_3d[:, 1:3] = -1 * labels_3d[:, 1:3] # TODO This is the temporary

                img, labels_2d_raw, offset, scale, _ = data_resize_with_cropped(img, labels_2d_raw)
                labels_2d -= offset[0:2]
                labels_2d /= scale

                example = tf.train.Example(features=tf.train.Features(
                    feature={
                        "image": tf.train.Feature(bytes_list=tf.train.BytesList(value=[img.tobytes()])),
                        "labels_2d": tf.train.Feature(float_list=tf.train.FloatList(value=labels_2d.flatten().tolist())),
                        "labels_3d": tf.train.Feature(float_list=tf.train.FloatList(value=labels_3d.flatten().tolist()))
                        }
                    ))

                train_writer.write(example.SerializeToString())
                sys.stderr.write("\rtraining data size: %d" % train_data_sum)
                train_data_sum += 1

    sys.stderr.write("\n")
    # generate test data
    test_data_sum = 0
    for test_file in test_dir_lists:
        reader.parse(test_file, camera_num)

        for cur_camera in range(camera_num):
            reader.reset()
            while reader.cur_frame_index < reader.total_frame_num:
                is_valid, img_path, labels, labels_raw = reader.getOneData(camera_num = cur_camera)
                img = cv2.imread(img_path)

                if is_ar:
                    labels_2d_raw = labels_raw[0].copy()
                else:
                    labels_2d_raw = labels[0].copy()

                labels_2d = labels[0].copy()
                labels_3d = labels[1].copy()
                img, labels_2d_raw, offset, scale, _ = data_resize_with_cropped(img, labels_2d_raw)

                labels_2d -= offset[0:2]
                labels_2d /= scale

                example = tf.train.Example(features=tf.train.Features(
                    feature={
                        "image": tf.train.Feature(bytes_list=tf.train.BytesList(value=[img.tobytes()])),
                        "labels_2d": tf.train.Feature(float_list=tf.train.FloatList(value=labels_2d.flatten().tolist())),
                        "labels_3d": tf.train.Feature(float_list=tf.train.FloatList(value=labels_3d.flatten().tolist()))
                        }
                    ))

                test_writer.write(example.SerializeToString())
                test_data_sum
                sys.stderr.write("\rvalid data size: %d" % test_data_sum)
                test_data_sum += 1

    sys.stderr.write("\n")

    train_writer.close()
    test_writer.close()
    print("Finished")
