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

from visual_tools import visualize_tools as vtools
from visual_tools import display_utils
from reader_scripts import dataReader as data_reader

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

    wndWidth = 960
    wndHeight = 720

    sleep_time = 1
    frame_num = -1

    reader = data_reader.mDataReader()
    total_camera_num = 6

    data_dir = "/home/kaihang/DataSet/sfu_selected_data/"
    data_list = os.listdir(data_dir)
    data_list = [os.path.join(data_dir, i) for i in data_list]

    ################################Scene Inited ######################################
    app = vtools.MyApplication(title = "train data show", wndWidth=wndWidth, wndHeight=wndHeight)
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
    cur_camera_num = 0
    cur_data_num = 0
    reader.parse(data_list[cur_data_num], total_camera_num)

    while not app.windowShouldClose():
        # time.sleep(sleep_time)
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
            # is_valid, img, annot2d, annot3d, annot3du = reader.getOneData()
            is_valid, img_path, label = reader.getOneData(camera_num = cur_camera_num)

            if is_valid:
                img = cv2.imread(img_path)
                annot2d = label[0]
                annot3d = label[1]
                img = display_utils.drawLines(img, annot2d)
                img = display_utils.drawPoints(img, annot2d)
            else:
                cur_camera_num += 1
                if reader.validCamera(cur_camera_num):
                    reader.reset()
                elif cur_data_num < len(data_list) - 1:
                    cur_data_num += 1
                    cur_camera_num = 0
                    reader.parse(data_list[cur_data_num], total_camera_num)
                else:
                    print("Valid data finished!")
                    quit()

        ###################################################################################
        mpose_model.draw(annot3d - annot3d[14] + np.array([0, 0, 3500]))
        cam_scene.drawFrame(img)
        app.renderEnd()

        if reset_rotate:
            reset_rotate = False
            mpose_model.rotate(reset=True)

    app.terminate()
