#Only use GPU:0
import os
os.environ["CUDA_DEVICE_ORDER"] = "PCI_BUS_ID"
os.environ["CUDA_VISIBLE_DEVICES"] = "1"

import numpy as np
import os
import sys
import cv2
import time

sys.path.append("../")

from reader_scripts import data_valid
from visual_tools import visualize_tools as vtools
from visual_tools import display_utils

########### Control the error and button and rotate
global init_x, init_y, cur_x, cur_y, is_mouse_pressed, now_save
cur_x = 0
cur_y = 0
is_mouse_pressed = False

keep_still = False
reset_rotate = False
global value_bonemaps
now_save = False

def scale_to_range(arr, min_val, max_val):
    arr = arr.copy()
    mask = arr != 0
    max_arr = np.max(arr[mask])
    min_arr = np.min(arr[mask])

    scale = float(max_val - min_val) / (max_arr - min_arr)
    arr[mask] = (arr[mask] - min_arr) * scale + min_val
    return arr

def mouse_down_callback(window, btn, action, mods):
    global init_x, init_y, cur_x, cur_y, is_mouse_pressed
    if (action == vtools.glfw.PRESS):
        if btn == vtools.glfw.MOUSE_BUTTON_LEFT:
            # print("Pressed the left key!")
            init_x = cur_x
            init_y = cur_y
            is_mouse_pressed = True
    else:
        if btn == vtools.glfw.MOUSE_BUTTON_LEFT:
            # print("Released the left key!")
            is_mouse_pressed = False

def mouse_move_callback(window, x, y):
    global cur_x, cur_y
    cur_x = x
    cur_y = y

def button_callback(window, key, scancode, action, mods):
    global keep_still
    global reset_rotate
    global now_save

    if (action != vtools.glfw.PRESS):
        return
    if key == vtools.glfw.KEY_SPACE:
        keep_still = not keep_still
    elif key == vtools.glfw.KEY_R:
        reset_rotate = True
    elif key == vtools.glfw.KEY_S:
        now_save = True
    elif key == vtools.glfw.KEY_ESCAPE:
        vtools.glfw.set_window_should_close(window, vtools.GL_TRUE)


def cv_mouse_callback(event, x, y, flags, ustc):
    global value_bonemaps
    if cv2.EVENT_MOUSEMOVE == event:
        text_show = np.zeros((50, 150), dtype=np.uint8)
        val = value_bonemaps[y, x]

        cv2.putText(text_show, str(val), (int(0), int(45)), cv2.FONT_HERSHEY_SIMPLEX, 1.5, (255, 255, 255), 1)
        cv2.imshow("value show", text_show)
        # cv2.waitKey(2)



if __name__ == "__main__":

    input_img_size = 368
    wndWidth = int(input_img_size)
    wndHeight = int(input_img_size)

    total_train_count = 14000
    total_valid_count = 1400
    total_frames = total_train_count
    tfrecord_file = "/home/kaihang/DataSet_2/MocapData/cmu_mocap/tfrecords/sfu_train.tfrecord"
    # tfrecord_file = "/home/kaihang/DataSet/mpi_tfrecords/mpi_train.tfrecords"
    ################################ Init tfrecord valider ##################################
    dataset_reader = data_valid.DataValid(tfrecord_file)
    #########################################################################################

    ################################Scene Inited ######################################
    app = vtools.MyApplication(title = "tfrecord test", wndWidth=wndWidth, wndHeight=wndHeight)
    app.glfwInit(mouse_down_callback = mouse_down_callback, mouse_move_callback=mouse_move_callback, button_callback = button_callback)

    cam_shader = vtools.ShaderReader("./shaders/v.shader", "./shaders/f.shader")
    model_shader = vtools.ShaderReader("./shaders/v2.shader", "./shaders/f2.shader")

    in_cammat_display = np.reshape([1497 / (2048 / wndWidth), 0.00000000e+00, wndWidth/2.0, 0.00000000e+00,
               0.00000000e+00, 1497 / (2048 / wndHeight), wndHeight/2.0, 0.00000000e+00,
               0.00000000e+00, 0.00000000e+00, 1.00000000e+00, 0.00000000e+00,
               0.00000000e+00, 0.00000000e+00, 0.00000000e+00, 1.00000000e+00], (4, 4))

    cam_scene = vtools.CamScene(cam_shader, wndWidth, wndHeight)

    mesh_render = vtools.MeshRender(model_shader, 30)
    mesh_render.addMesh("./models/sphere-30.ply")

    mpose_model = vtools.PoseModel(in_cammat_display, mesh_render, (wndWidth, wndHeight), True)
    ####################################################################################

    frame_count = 0
    max_x = -100000
    min_x = 100000
    max_y = -100000
    min_y = 100000
    max_z = -100000
    min_z = 100000

    while not app.windowShouldClose() and frame_count < total_frames:
        # time.sleep(0.05)
        app.renderStart()
        ###################### handle the rotate event##################
        if is_mouse_pressed:
            div_x = cur_x - init_x
            div_y = cur_y - init_y

            if div_x != 0 or div_y != 0:
                # point_from = np.array([wndWidth - init_x, wndHeight - init_y, 0])
                # point_to = np.array([wndWidth - cur_x, wndHeight - cur_y, 0])

                point_from = np.array([wndWidth - init_x, 0, 0])
                point_to = np.array([wndWidth - cur_x, 0, 0])

                origin_p = (point_to + point_from) / 2
                origin_p[2] = - wndWidth / 10.0
                vec_from = vtools.OpenGLUtils.normalize(point_from - origin_p)
                vec_to = vtools.OpenGLUtils.normalize(point_to - origin_p)

                axis = vtools.OpenGLUtils.normalize(np.cross(vec_from, vec_to))
                theta = np.arccos(np.dot(vec_from, vec_to))

                rotateMat = vtools.OpenGLUtils.rotate(axis, theta, True)
                mpose_model.rotate(rotateMat)
        else:
            mpose_model.rotate(set_prev=True)
        #################################################################

        #############################Get Images and points ################################
        if not keep_still:

            img, points_2d, points_3d = dataset_reader.get_frame()
            print(points_2d)
            # bones_length = np.zeros([14])

            # bones_length[0] = np.linalg.norm(points_3d[0] - points_3d[1])

            # bones_length[1] = np.linalg.norm(points_3d[1] - points_3d[2])
            # bones_length[2] = np.linalg.norm(points_3d[2] - points_3d[3])
            # bones_length[3] = np.linalg.norm(points_3d[3] - points_3d[4])

            # bones_length[4] = np.linalg.norm(points_3d[1] - points_3d[5])
            # bones_length[5] = np.linalg.norm(points_3d[5] - points_3d[6])
            # bones_length[6] = np.linalg.norm(points_3d[6] - points_3d[7])

            # bones_length[7] = np.linalg.norm(points_3d[1] - points_3d[14])

            # bones_length[8] = np.linalg.norm(points_3d[14] - points_3d[8])
            # bones_length[9] = np.linalg.norm(points_3d[8] - points_3d[9])
            # bones_length[10] = np.linalg.norm(points_3d[9] - points_3d[10])

            # bones_length[11] = np.linalg.norm(points_3d[14] - points_3d[11])
            # bones_length[12] = np.linalg.norm(points_3d[11] - points_3d[12])
            # bones_length[13] = np.linalg.norm(points_3d[12] - points_3d[13])

            # print(bones_length)

            # spin_len = np.linalg.norm(points_3d[1] - points_3d[14])
            # min_x = min(np.min(points_3d[:, 0]), min_x)
            # max_x = max(np.max(points_3d[:, 0]), max_x)

            # min_y = min(np.min(points_3d[:, 1]), min_y)
            # max_y = max(np.max(points_3d[:, 1]), max_y)

            # min_z = min(np.min(points_3d[:, 2]), min_z)
            # max_z = max(np.max(points_3d[:, 2]), max_z)

            # print(frame_count, (min_x, max_x), (min_y, max_y), (min_z, max_z))
            # print(points_3d)

            img = display_utils.drawLines(img, points_2d)
            img = display_utils.drawPoints(img, points_2d)

            frame_count += 1

            points_3d += [0, 0, 3000]
        ###################################################################################

        mpose_model.draw(points_3d)
        cam_scene.drawFrame(img)

        app.renderEnd()
        if reset_rotate:
            reset_rotate = False
            mpose_model.rotate(reset=True)

    app.terminate()
