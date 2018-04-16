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

from utils import data_valid
from cpm_utils import visualize_tools as vtools
from cpm_utils import display_utils

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
    # 8->376, 6->585, 7->420 5->760

    frame_stop_num = 743
    obj_num = 5
    use_saved_data = True

    input_img_size = 368
    wndWidth = int(input_img_size)
    wndHeight = int(input_img_size)

    tfrecord_file = "/home/kaihang/DataSet_2/mpii_valid_problem/"+str(obj_num)+"/mpi-train-v8s2.tfrecords"
    data_type = 3

    ################################ Init tfrecord valider ##################################
    dataset_reader = data_valid.DataValid(tfrecord_file, data_type)
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

    ########################### For visualization #############################
    if data_type == 3:
        global value_bonemaps
        bm_min_val = -200
        bm_max_val = 200
        bm_mid_val = 0

        cv2.imshow("Range map", display_utils.getHeatmapRange(bm_min_val, bm_mid_val, bm_max_val))
        cv2.waitKey(3)

        cv2.namedWindow("bonemap show")
        cv2.setMouseCallback("bonemap show", cv_mouse_callback)
        # cv2.setMouseCallback("tfrecord test", cv_mouse_callback)

        cv2.imshow("value show", np.zeros((50, 150), dtype=np.uint8))
    ###########################################################################

    frame_count = 0
    while not app.windowShouldClose():
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
            print(frame_count)


            # if frame_count == 119:
            if frame_count == frame_stop_num:
                keep_still = True

            if data_type == 3:
                if use_saved_data:
                    saved_data = np.load(str(obj_num) + "-" + str(frame_stop_num) + ".npy").tolist()
                    img = saved_data["img"]
                    points_2d = saved_data["points_2d"]
                    points_3du = saved_data["points_3du"]
                    bonemaps = saved_data["bonemaps"]
                else:
                    img, points_2d, points_3du, bonemaps = dataset_reader.get_frame()

                if now_save:
                    now_save = False
                    np.save(str(obj_num) + "-" + str(frame_count), {"img":img, "points_2d": points_2d, "points_3du": points_3du, "bonemaps": bonemaps})

                img = display_utils.drawLines(img, points_2d)
                img = display_utils.drawPoints(img, points_2d)
            else:
                img, points_2d = dataset_reader.get_frame()
                for cur_point in points_2d:
                    img = display_utils.drawLines(img, cur_point)
                    img = display_utils.drawPoints(img, cur_point)

            frame_count += 1

        ###################################################################################
        if data_type == 3:
            mpose_model.draw(points_3du)
            # visualize bonemap
            # bonemaps = cv2.resize(bonemaps, (368, 368), interpolation=cv2.INTER_NEAREST)
            bm_forshow, value_bonemaps = display_utils.visualizeBonemaps(bonemaps, min_val = bm_min_val, mid_val = bm_mid_val, max_val = bm_max_val)
            value_bonemaps = cv2.resize(value_bonemaps, (368, 368), interpolation=cv2.INTER_NEAREST)
            cv2.imshow("bonemap show", cv2.resize(bm_forshow, (368, 368), interpolation=cv2.INTER_NEAREST))
            # cv2.imshow("bonemap show", bm_forshow)
            cv2.waitKey(3)

        cam_scene.drawFrame(img)

        app.renderEnd()

        if reset_rotate:
            reset_rotate = False
            mpose_model.rotate(reset=True)

    app.terminate()
