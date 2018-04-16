import tensorflow as tf
import numpy as np
import os

def read_and_decode_3d(tfr_queue, img_size, bm_size, num_joints):
    tfr_reader = tf.TFRecordReader()
    _, serialized_example = tfr_reader.read(tfr_queue)

    features = tf.parse_single_example(serialized_example,
                                       features={
                                           'image': tf.FixedLenFeature([], tf.string),
                                           'labels_3d': tf.FixedLenFeature([3* num_joints], tf.float32),
                                           'labels_2d': tf.FixedLenFeature([2 * num_joints], tf.float32)
                                       })

    img = tf.decode_raw(features['image'], tf.uint8)
    img = tf.reshape(img, [img_size, img_size, 3])

    labels_3d = tf.reshape(features['labels_3d'], (num_joints, 3))
    labels_2d = tf.reshape(features['labels_2d'], (num_joints, 2))

    return [img], [labels_2d], [labels_3d]

def read_batch_3d(tfr_path, img_size, bm_size, num_joints, batch_size=4, num_epochs=None, is_testing=False, is_shuffle=True):

    for i in range(len(tfr_path)):
        if not os.path.exists(tfr_path[i]):
            raise PathError(tfr_path[i])

    with tf.name_scope('Batch_Inputs'):
        if not is_testing:
            # Traing
            print("Open training file queue!")
            tfr_queue = tf.train.string_input_producer(tfr_path, num_epochs=num_epochs, shuffle=is_shuffle)
            data_list = [read_and_decode_3d(tfr_queue, img_size, bm_size, num_joints) for _ in
                         range(1 * len(tfr_path))]

            if is_shuffle:
                batch_images, batch_labels_2d, batch_labels_3d = tf.train.shuffle_batch_join(data_list,
                                                                                            batch_size=batch_size,
                                                                                            capacity=400,
                                                                                            min_after_dequeue=80,
                                                                                            enqueue_many=True,
                                                                                            name='batch_data_read_train')
            else:
                batch_images, batch_labels_2d, batch_labels_3d = tf.train.batch_join(data_list,
                                                                                            batch_size=batch_size,
                                                                                            capacity=400,
                                                                                            enqueue_many=True,
                                                                                            name='batch_data_read_train')

        else:
            # Testing
            print("Open testing file queue!")
            tfr_queue = tf.train.string_input_producer(tfr_path, num_epochs=num_epochs, shuffle=is_shuffle)
            data_list = [read_and_decode_3d(tfr_queue, img_size, bm_size, num_joints) for _ in
                         range(1 * len(tfr_path))]

            if is_shuffle:
                batch_images, batch_labels_2d, batch_labels_3d = tf.train.shuffle_batch_join(data_list,
                                                                                             batch_size=batch_size,
                                                                                             capacity=400,
                                                                                             min_after_dequeue=80,
                                                                                             enqueue_many=True,
                                                                                             name='batch_data_read_test')
            else:
                batch_images, batch_labels_2d, batch_labels_3d = tf.train.batch_join(data_list,
                                                                                     batch_size=batch_size,
                                                                                     capacity=400,
                                                                                     enqueue_many=True,
                                                                                     name='batch_data_read_test')

    return batch_images, batch_labels_2d, batch_labels_3d
