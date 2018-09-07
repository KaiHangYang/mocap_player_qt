import numpy as np
import cv2
import os
import tensorflow as tf
import sys

sys.path.append("../")
sys.path.append("/home/kaihang/Projects/mocap_pose_tf/cpm_utils/visualize_utils/")

from reader_scripts import dataReader as data_reader
import display_utils

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

def crop_n_resize_joints(joints_2d, pad_scale=0.2, target_size = 368):
    max_x = np.max(joints_2d[:, 0])
    min_x = np.min(joints_2d[:, 0])

    max_y = np.max(joints_2d[:, 1])
    min_y = np.min(joints_2d[:, 1])

    center = np.array([(min_x + max_x) / 2.0, (min_y + max_y) / 2.0])
    size = max((max_x - min_x), (max_y - min_y)) * (1 + pad_scale)

    result = [0, 0, 1.0]
    result[0] = center[0] - size / 2
    result[1] = center[1] - size / 2
    result[2] = target_size / size

    return result


if __name__ == "__main__":

    settings = {
            "is_ar": False,
            "camera_num": 36,
            "train_data_path": "/home/kaihang/DataSet_2/MocapData/sfu_mocap/sfu_mocap_result/36_camera/datas/synthesis/type_3/train",
            "valid_data_path": "/home/kaihang/DataSet_2/MocapData/sfu_mocap/sfu_mocap_result/36_camera/datas/synthesis/type_3/valid",
            "train_tfrecord_path": "/home/kaihang/DataSet_2/MocapData/sfu_mocap/sfu_mocap_result/36_camera/tfrecords/train_mpii_syn_368_together.tfrecord",
            "valid_tfrecord_path": "/home/kaihang/DataSet_2/MocapData/sfu_mocap/sfu_mocap_result/36_camera/tfrecords/valid_mpii_syn_368_together.tfrecord"
            }

    dataset_name = ["train", "valid"]

    for dn in dataset_name:

        is_ar = settings["is_ar"]
        camera_num = settings["camera_num"]
        data_path = settings[dn + "_data_path"]
        tfr_writer = tf.python_io.TFRecordWriter(settings[dn + "_tfrecord_path"])
        dataset_list = os.listdir(data_path)
        dataset_list.sort()

        print(dataset_list)

        dir_lists = [os.path.join(data_path, i) for i in dataset_list]

        reader = data_reader.mDataReader(is_ar)
        # generate data
        data_sum = 0
        for cur_dir in dir_lists:
            reader.parse(cur_dir, camera_num)
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

                    ###### Visualize the data ######
                    # show_img = display_utils.drawLines(img, labels_2d)
                    # show_img = display_utils.drawPoints(img, labels_2d)

                    # cv2.imshow("together", show_img)
                    # cv2.waitKey(0)
                    ################################

                    # offset_n_scale = crop_n_resize_joints(labels_2d_raw, pad_scale = pad_scale, target_size = target_image_size)
                    # labels_2d -= offset_n_scale[0:2]
                    # labels_2d *= offset_n_scale[2]

                    example = tf.train.Example(features=tf.train.Features(
                        feature={
                            "image": tf.train.Feature(bytes_list=tf.train.BytesList(value=[img.tobytes()])),
                            "labels_2d": tf.train.Feature(float_list=tf.train.FloatList(value=labels_2d.flatten().tolist())),
                            "labels_3d": tf.train.Feature(float_list=tf.train.FloatList(value=labels_3d.flatten().tolist()))
                            }
                        ))

                    tfr_writer.write(example.SerializeToString())
                    sys.stderr.write("\r%s data size: %d" % (dn, data_sum))
                    data_sum += 1

        tfr_writer.close()

        sys.stderr.write("\n")
    sys.stderr.write("\n")

    print("Finished")

