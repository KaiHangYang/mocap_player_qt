import sys
import numpy as np
import tensorflow as tf

sys.path.append("../")

import tfrecords_reader as tfr_reader

class DataValid():
    def __init__(self, data_file, img_size = 368, num_of_joint = 15):
        self.batch_img, self.batch_labels_2d, self.batch_labels_3d = tfr_reader.read_batch_3d([data_file],
            img_size, img_size / 8, num_of_joint, batch_size = 1, num_epochs = None, is_testing = False, is_shuffle = False)

        # Don't use all the memory of GPU
        gpu_options = tf.GPUOptions(per_process_gpu_memory_fraction=0.1)

        self.sess = tf.Session(config=tf.ConfigProto(gpu_options=gpu_options))
        self.img_size = img_size
        self.num_of_joint = num_of_joint

        with self.sess.as_default():
            self.coord = tf.train.Coordinator()
            self.threads = tf.train.start_queue_runners(coord=self.coord)

    def get_frame(self):
        with self.sess.as_default():

            batch_img_np, batch_labels_2d_np, batch_labels_3d_np = self.sess.run([self.batch_img, self.batch_labels_2d, self.batch_labels_3d])

            img = batch_img_np[0].astype(np.uint8).copy()
            point_list_2d = batch_labels_2d_np[0].copy()
            point_list_3d = batch_labels_3d_np[0].copy()

            return img, point_list_2d, np.reshape(point_list_3d, (-1, 3))

    def __del__(self):
        self.coord.request_stop()
        self.coord.join(self.threads)
