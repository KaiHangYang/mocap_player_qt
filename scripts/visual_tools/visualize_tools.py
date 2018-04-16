import numpy as np

from OpenGL.GL import *
from OpenGL import *
from OpenGL.GLUT import *
from OpenGL.GLU import *

import ctypes
import glfw

import os
import sys
import cv2 as cv
import pyassimp as assimp

class PathError(Exception):
    def __init__(self, value):
        self.value = value

    def __str__(self):
        return self.value + ': File is not existing!'

def error_callback(error, desc):
    print("glfw Error(%d): %s" % (error, desc))

class MyApplication():
    def __init__(self, title="Application", wndWidth=368, wndHeight=368, is_visible=True):
        self.title = title
        self.wndHeight = wndHeight
        self.wndWidth = wndWidth
        self.window = None
        self.is_visible = is_visible

    def glfwInit(self, button_callback = None, mouse_down_callback = None, mouse_move_callback = None):

        glfw.set_error_callback(error_callback)

        if not glfw.init():
            print("glfw Error: init glfw failed!")
            return False
        glfw.window_hint(glfw.RESIZABLE, GL_FALSE)
        glfw.window_hint(glfw.SAMPLES, 1)
        glfw.window_hint(glfw.CONTEXT_VERSION_MAJOR, 3)
        glfw.window_hint(glfw.CONTEXT_VERSION_MINOR, 3)
        glfw.window_hint(glfw.OPENGL_FORWARD_COMPAT, GL_TRUE)
        glfw.window_hint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE)

        if self.is_visible:
            glfw.window_hint(glfw.VISIBLE, GL_TRUE)
        else:
            glfw.window_hint(glfw.VISIBLE, GL_FALSE)

        self.window = glfw.create_window(self.wndWidth, self.wndHeight, self.title, None, None)
        if not self.window:
            print("glfw Error: create window failed!")
            glfw.terminate()
            return False

        glfw.make_context_current(self.window)
        glfw.swap_interval(0)
        glfw.set_input_mode(self.window, glfw.STICKY_KEYS, GL_TRUE)

        if button_callback is not None and hasattr(button_callback, "__call__"):
            glfw.set_key_callback(self.window, button_callback)
        if mouse_down_callback is not None and hasattr(mouse_down_callback, "__call__"):
            glfw.set_mouse_button_callback(self.window, mouse_down_callback)
        if mouse_move_callback is not None and hasattr(mouse_move_callback, "__call__"):
            glfw.set_cursor_pos_callback(self.window, mouse_move_callback)

        # Enable the Depth test
        glEnable(GL_DEPTH_TEST)
        glDisable(GL_MULTISAMPLE)
        glEnable(GL_PROGRAM_POINT_SIZE)
        glDepthFunc(GL_LESS)
        # glEnable(GL_LINE_SMOOTH)
        return True
    def windowShouldClose(self):
        return glfw.window_should_close(self.window)

    def renderStart(self):
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glClearColor(0.0, 0.0, 0.0, 1.0)

    def renderEnd(self):
        glfw.swap_buffers(self.window)
        glfw.poll_events()

    def terminate(self):
        glfw.terminate()


class OpenGLUtils():
    def __init__(self):
        pass

    @staticmethod
    def perspective(fov, ratio, near, far, is_transpose = False):
        matrix = np.zeros((4, 4))
        matrix[1][1] = 1.0 / np.tan(fov/2.0)
        matrix[0][0] = matrix[1][1] / ratio
        matrix[3][2] = -1
        matrix[2][2] = -(far + near) / (far - near)
        matrix[2][3] = -2 * far * near / (far - near)

        if not is_transpose:
            return np.transpose(matrix, [1, 0])
        else:
            return matrix
    @staticmethod
    def ortho(left, right, bottom, top, near, far, is_transpose=False):
        tx = -float(right + left) / (right - left)
        ty = -float(top + bottom) / (top - bottom)
        tz = -float(far + near) / (far - near)

        mat = np.array([
            [2.0/(right - left), 0, 0, tx],
            [0, 2.0/(top - bottom), 0, ty],
            [0, 0, -2.0/(far - near), tz],
            [0, 0, 0, 1]
            ])

        if not is_transpose:
            return np.transpose(mat, [1, 0])
        else:
            return mat
    # input numpy.array
    @classmethod
    def normalize(cls, data):
        data_sum = pow(sum([i*i for i in data]), 0.5)
        result = np.array(data) / data_sum
        return result

    @classmethod
    def lookAt(cls, eye, target, up, is_transpose=False):
        eye = np.array(eye)
        target = np.array(target)
        up = np.array(up)
        # TODO I can't understand why it's that
        Z = cls.normalize(eye - target)
        X = cls.normalize(np.cross(up, Z))
        Y = np.cross(Z, X)

        mat = np.array([
            [X[0], Y[0], Z[0], 0.0],
            [X[1], Y[1], Z[1], 0.0],
            [X[2], Y[2], Z[2], 0.0],
            [np.dot(X, -eye), np.dot(Y, -eye), np.dot(Z, -eye), 1.0]
            ])
        if not is_transpose:
            return np.transpose(mat, [1,0])
        else:
            return mat
    @classmethod
    def translate(cls, target, is_transpose=False):
        mat = np.array([
            [1, 0, 0, target[0]],
            [0, 1, 0, target[1]],
            [0, 0, 1, target[2]],
            [0, 0, 0, 1]
            ], dtype=np.float32)
        if not is_transpose:
            return np.transpose(mat, [1, 0])
        else:
            return mat

    @classmethod
    def rotate(cls, axis, theta, is_transpose=False):
        axis = cls.normalize(axis)
        u = axis[0]
        v = axis[1]
        w = axis[2]

        mat = np.zeros((4, 4), dtype=np.float32)

        mat[0][0] = np.cos(theta) + (u * u) * (1 - np.cos(theta));
        mat[0][1] = u * v * (1 - np.cos(theta)) + w * np.sin(theta);
        mat[0][2] = u * w * (1 - np.cos(theta)) - v * np.sin(theta);
        mat[0][3] = 0;

        mat[1][0] = u * v * (1 - np.cos(theta)) - w * np.sin(theta);
        mat[1][1] = np.cos(theta) + v * v * (1 - np.cos(theta));
        mat[1][2] = w * v * (1 - np.cos(theta)) + u * np.sin(theta);
        mat[1][3] = 0;

        mat[2][0] = u * w * (1 - np.cos(theta)) + v * np.sin(theta);
        mat[2][1] = v * w * (1 - np.cos(theta)) - u * np.sin(theta);
        mat[2][2] = np.cos(theta) + w * w * (1 - np.cos(theta));
        mat[2][3] = 0;

        mat[3][0] = 0;
        mat[3][1] = 0;
        mat[3][2] = 0;
        mat[3][3] = 1;
        if not is_transpose:
            return mat
        else:
            return np.transpose(mat, [1, 0])
    @classmethod
    def scale(cls, ratio):
        return np.array([
            [ratio[0], 0, 0, 0],
            [0, ratio[1], 0, 0],
            [0, 0, ratio[2], 0],
            [0, 0, 0, 1]
            ])

class ShaderReader():
    def __init__(self, v_path = None, f_path = None):
        self.program = glCreateProgram()

        if not v_path or not f_path or not os.path.exists(v_path) or not os.path.exists(f_path):
            print("Error: The vertex shader path or the fragment shader path is not valid!")
            raise PathError("Shader file")

        with open(v_path) as v_file:
            vertexShaderCode = v_file.read()
        with open(f_path) as f_file:
            fragShaderCode = f_file.read()
        self.vertexShader = self.create_shader(vertexShaderCode, GL_VERTEX_SHADER)
        self.fragShader = self.create_shader(fragShaderCode, GL_FRAGMENT_SHADER)

        glAttachShader(self.program, self.vertexShader)
        glAttachShader(self.program, self.fragShader)

        glLinkProgram(self.program)
        message = self.get_program_log(self.program)
        if message:
            print("Shader: shader program message: %s" % message)

    def create_shader(self, source, shadertype):
        shader = glCreateShader(shadertype)
        if isinstance(source, basestring):
            source = [source]

        glShaderSource(shader, source)
        glCompileShader(shader)
        message = self.get_shader_log(shader)
        if message:
            print("Shader: shader message: %s" % message)

        return shader

    def get_shader_log(self, shader):
        return self.get_log(shader, glGetShaderInfoLog)

    def get_program_log(self, shader):
        return self.get_log(shader, glGetProgramInfoLog)

    def get_log(self, obj, func):
        value = func(obj)
        return value

    def use(self):
        glUseProgram(self.program)

    def stop(self):
        glUseProgram(0)

    def set_uniform_f(self, name, value):
        location = glGetUniformLocation(self.program, name)
        glUniform1f(location, value)

    def set_uniform_i(self, name, value):
        location = glGetUniformLocation(self.program, name)
        glUniform1i(location, value)

    def set_uniform_ui(self, name, value):
        location = glGetUniformLocation(self.program, name)
        glUniform1ui()

    def set_uniform_m4(self, name, value):
        location = glGetUniformLocation(self.program, name)
        glUniformMatrix4fv(location, 1, GL_FALSE, value)

    def set_uniform_v3(self, name, value):
        location = glGetUniformLocation(self.program, name)
        glUniform3fv(location, 1, value)

    def __setitem__(self, name, value):
        if len(name) == 2:
            if name[1] == (1, 3):
                self.set_uniform_v3(name[0], value)
        else:
            if isinstance(value, float) or isinstance(value, np.float32):
                # print("Set uniform float data!")
                self.set_uniform_f(name, value)
            elif isinstance(value, int):
                # print("Set uniform int data!")
                self.set_uniform_i(name, value)
            elif len(value) == 3:
                # print("Set uniform mat3 data!")
                self.set_uniform_v3(name, value)
            elif len(value) == 4:
            # print("Set uniform mat4 data!")
                self.set_uniform_m4(name, value)

class MeshEntry():
    def __init__(self, vertexs, faces, norms, vao):
        self.VAO = vao
        self.elmNum = len(faces)
        glBindVertexArray(self.VAO)

        # put in the vertexs
        self.vertex_buffer = glGenBuffers(1)
        glBindBuffer(GL_ARRAY_BUFFER, self.vertex_buffer)
        glBufferData(GL_ARRAY_BUFFER, vertexs.flatten(), GL_STATIC_DRAW)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, ctypes.c_void_p(0))

        # put in the indices
        self.indices_buffer = glGenBuffers(1)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self.indices_buffer)
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, np.array(faces, dtype=np.uint32).flatten(), GL_STATIC_DRAW)

        # put in the norms
        self.norm_buffer = glGenBuffers(1)
        glBindBuffer(GL_ARRAY_BUFFER, self.norm_buffer)
        glBufferData(GL_ARRAY_BUFFER, norms.flatten(), GL_STATIC_DRAW)
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, ctypes.c_void_p(0))


        glBindBuffer(GL_ARRAY_BUFFER, 0)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0)
        glBindVertexArray(0)

    def render(self):
        glBindVertexArray(self.VAO)

        glBindBuffer(GL_ARRAY_BUFFER, self.vertex_buffer)
        glEnableVertexAttribArray(0)

        glBindBuffer(GL_ARRAY_BUFFER, self.norm_buffer)
        glEnableVertexAttribArray(1)

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self.indices_buffer)

        glDrawElements(GL_TRIANGLES, 3*self.elmNum, GL_UNSIGNED_INT, ctypes.c_void_p(0))
        glDisableVertexAttribArray(0)
        glDisableVertexAttribArray(1)
        glBindBuffer(GL_ARRAY_BUFFER, 0)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0)
        glBindVertexArray(0)

class MeshRender():
    def __init__(self, shader, sphere_ratio):
        self.shader = shader
        self.VAO = glGenVertexArrays(1)
        self.meshes = []
        # TODO the used sphere ratio
        self.sphere_ratio = sphere_ratio

    def addMesh(self, mesh_path):
        scene = assimp.load(mesh_path)
        mesh = scene.meshes[0]
        self.meshes.append(MeshEntry(mesh.vertices, mesh.faces, mesh.normals, self.VAO))

    def render(self, vertexs, indices, proj_mat):
        glBindVertexArray(self.VAO)

        vertex_num = len(vertexs) / 3
        line_num = len(indices) / 2

        vertex_flags = [0] * vertex_num
        self.shader.use()
        # use the realword unit
        self.shader["viewPos", (1, 3)] = np.array([0, 0, -100])
        self.shader["lightPos", (1, 3)] = np.array([10000, -10000, -10000])
        self.shader["fragColor", (1, 3)] = np.array([1.0, 0.94, 0.87])
        # self.shader["fragColor", (1, 3)] = np.array([1.0, 0, 0])

        for i in range(line_num):
            line = [indices[2 * i], indices[2 * i + 1]]
            model_mat = np.identity(4)

            for j in range(2):
                if not vertex_flags[line[j]]:
                    vertex_flags[line[j]] = 1

                    trans_mat = OpenGLUtils.translate([vertexs[3 * line[j]], vertexs[3 * line[j] + 1], vertexs[3 * line[j] + 2]], True)
                    scale_mat = OpenGLUtils.scale([1, 1, 1])

                    cur_model_mat = np.dot(trans_mat, scale_mat)
                    model_mat = np.transpose(cur_model_mat, [1, 0])
                    self.shader["MVP"] = np.transpose(np.dot(proj_mat, cur_model_mat))

                    self.shader["modelMat"] = model_mat
                    self.shader["normMat"] = np.linalg.inv(cur_model_mat)
                    self.meshes[0].render()
                    # render the dot mesh
            point_a = np.array([vertexs[3 * line[0]], vertexs[3 * line[0] + 1], vertexs[3 * line[0] + 2]])
            point_b = np.array([vertexs[3 * line[1]], vertexs[3 * line[1] + 1], vertexs[3 * line[1] + 2]])
            line_center = (point_a + point_b) / 2.0
            length = np.sqrt(sum((point_a - point_b) ** 2))
            # then render the line mesh
            vFrom = np.array([0, 0, 1])
            vTo = OpenGLUtils.normalize(point_a - point_b)

            trans_mat = OpenGLUtils.translate([line_center[0], line_center[1], line_center[2]], True)
            angle = np.arccos(np.dot(vFrom, vTo))
            if angle <= 0.000001:
                rotate_mat = np.identity(4)
            else:
                rotate_mat = OpenGLUtils.rotate(OpenGLUtils.normalize(np.cross(vFrom, vTo)), angle, True)

            scale_mat = OpenGLUtils.scale([1, 1, length / (2.0 * self.sphere_ratio)])
            curmodel = np.dot(trans_mat, np.dot(rotate_mat, scale_mat))
            tran_curmodel = np.transpose(curmodel, [1, 0])
            self.shader["MVP"] = np.transpose(np.dot(proj_mat, curmodel))

            self.shader["modelMat"] = tran_curmodel
            self.shader["normMat"] = np.linalg.inv(curmodel)
            self.meshes[0].render()

class PoseModel():
    def __init__(self, cam_matrix, mesh_render, cam_win_size, is_ar = True):
        self.mesh_render = mesh_render
        self.limbs = np.array([
            0, 1,
            1, 2,
            2, 3,
            3, 4,
            1, 5,
            5, 6,
            6, 7,
            1, 14,
            8, 14,
            8, 9,
            9, 10,
            14, 11,
            11, 12,
            12, 13
        ], dtype=np.uint8)
        self.rotateMat = np.identity(4)
        self.prev_rotateMat = np.identity(4)
        self.cam_img_width = cam_win_size[0]
        self.cam_img_height = cam_win_size[1]
        # self.num_points = 15

        # write the calculateed project&clip matrix
        # for the real camera the far is infinity and near is f
        if is_ar:
            self.Proj = np.array([
                [2.0*cam_matrix[0][0] / self.cam_img_width, 0, -1 + 2.0*cam_matrix[0][2] / self.cam_img_width, 0.0],
                [0, -2.0*cam_matrix[1][1]/self.cam_img_height, 1 - 2.0*cam_matrix[1][2] / self.cam_img_height, 0.0],
                [0, 0, 1, -2 * cam_matrix[0][0]],
                [0, 0, 1, 0]
                ], dtype=np.float32)

            print(self.Proj)
        else:
            self.Proj = cam_matrix

    def rotate(self, rotateMat=np.identity(4), reset=False, set_prev=False):
        global is_mouse_pressed
        if reset:
            self.rotateMat = np.identity(4)
        else:
            if not set_prev:
                self.rotateMat = np.dot(rotateMat, self.prev_rotateMat)
            else:
                self.prev_rotateMat = self.rotateMat

    def draw(self, points):
        # I get that, in the vertex shader OpenGL don't divide the w
        points = points.copy()
        # rotate the points
        origin_p = points[14].copy()
        for i in range(points.shape[0]):
            points[i] -= origin_p
            points[i] = np.dot(self.rotateMat, np.concatenate((points[i], [1.0])))[0:3]
            points[i] += origin_p
        points = points.flatten()
        self.mesh_render.render(points, self.limbs, self.Proj)
        # self.mesh_render.render(points, [0, 1], self.Proj)

class CamScene():
    def __init__(self, cam_shader, wndWidth, wndHeight):
        self.wndWidth = wndWidth
        self.wndHeight = wndHeight
        self.cam_shader = cam_shader
        self.base_fov = 45.0
        self.VAO = glGenVertexArrays(1)
        self.z_pos = 99.0
        self.proj_mat = None

        glBindVertexArray(self.VAO)
        self.initGLFrame()
        glBindVertexArray(0)

    def initGLFrame(self):
        projMat = OpenGLUtils.perspective(np.radians(self.base_fov), float(self.wndWidth) / self.wndHeight, 0.1, 100.0)
        self.proj_mat = projMat
        modelMat = np.identity(4)
        self.MVP = np.dot(projMat, modelMat)
        self.genTexture()
        tmp_t = self.z_pos * np.tan(np.radians(self.base_fov/2))
        tmp_r = tmp_t * self.wndWidth / self.wndHeight
        vertex_buffer_data = np.array([
            -tmp_r, tmp_t, -self.z_pos,
            -tmp_r, -tmp_t, -self.z_pos,
            tmp_r, -tmp_t, -self.z_pos,
            tmp_r, -tmp_t, -self.z_pos,
            tmp_r, tmp_t, -self.z_pos,
            -tmp_r, tmp_t, -self.z_pos], dtype=np.float32)
        uv_data = np.array([
                1.0, 1.0,
                1.0, 0.0,
                0.0, 0.0,
                0.0, 0.0,
                0.0, 1.0,
                1.0, 1.0], dtype=np.float32)

        self.vertexBuffer = glGenBuffers(1)
        glBindBuffer(GL_ARRAY_BUFFER, self.vertexBuffer)
        glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data, GL_STATIC_DRAW)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, ctypes.c_void_p(0))

        self.uvBuffer = glGenBuffers(1)
        glBindBuffer(GL_ARRAY_BUFFER, self.uvBuffer)
        glBufferData(GL_ARRAY_BUFFER, uv_data, GL_STATIC_DRAW)
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, ctypes.c_void_p(0))


    def genTexture(self):
        self.textureID = glGenTextures(1)
        glBindTexture(GL_TEXTURE_2D, self.textureID)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT)

    def setTextureData(self, frame):
        frame = cv.flip(frame, -1)
        frame = frame.tobytes()
        glBindTexture(GL_TEXTURE_2D, self.textureID)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, self.wndWidth, self.wndHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, frame)
        glGenerateMipmap(GL_TEXTURE_2D)

    def drawFrame(self, frame):
        glBindVertexArray(self.VAO)
        self.cam_shader.use()
        self.cam_shader["MVP"] = self.MVP

        frame = cv.resize(frame, (self.wndWidth, self.wndHeight))
        self.setTextureData(frame)

        # The default activated texture is 0, so this line is not necessary
        glActiveTexture(GL_TEXTURE0)
        glBindTexture(GL_TEXTURE_2D, self.textureID)
        self.cam_shader["myTextureSampler"] = 0

        glEnableVertexAttribArray(0)
        glEnableVertexAttribArray(1)

        glDrawArrays(GL_TRIANGLES, 0, 6)
        glDisableVertexAttribArray(0)
        glDisableVertexAttribArray(1)
        glBindVertexArray(0)


class BBTracker():
    def __init__(self, wndWidth = 368, wndHeight = 368, pad_scale = 1.3):
        self.pad_scale = pad_scale
        self.crop_box_size = 368
        self.wndWidth = wndWidth
        self.wndHeight = wndHeight
        self.center = np.asarray([wndWidth / 2.0, wndHeight / 2.0])
        self.box_size = max([wndWidth, wndHeight])
        self.scale = float(self.box_size) / self.crop_box_size
        self.offset = [(self.wndWidth - self.box_size) / 2.0, (self.wndHeight - self.box_size) / 2.0]
        self.initial_frame_num = 5
        self.frame_num = 0
        # self.bbx = [self.center[0] - self.box_size / 2.0, self.center[0] + self.box_size / 2.0, self.center[1] - self.box_size / 2.0, self.center[1] + self.box_size / 2.0]
        self.bbx = [0, 0, wndWidth, wndHeight]
        self.for_test = 0

        self.r_val = [0] * 4
        self.t_val = [0] * 4

    def track(self, points, points_belief, img):
        points = np.reshape(points, (-1, 2))

        points *= self.scale
        points[:, 0] += self.offset[0]
        points[:, 1] += self.offset[1]

        img_width = img.shape[1]
        img_height = img.shape[0]

        min_x = np.min(points[:, 0])
        min_y = np.min(points[:, 1])
        max_x = np.max(points[:, 0])
        max_y = np.max(points[:, 1])

        box_size = max([max_x - min_x, max_y - min_y]) * self.pad_scale
        center = np.asarray([(max_x + min_x) / 2.0, (max_y + min_y) / 2.0])

        if self.frame_num > self.initial_frame_num:
            # box_size = int(self.box_size * 0.3 + box_size * 0.7)
            # center = self.center * 0.3 + center * 0.7
            if pow((self.center[0] - center[0]) ** 2 + (self.center[1] - center[1]) ** 2, 0.5) > 10:
                box_size = int(self.box_size * 0.4 + box_size * 0.6)
                center = self.center * 0.4 + center * 0.6
            else:
                box_size = self.box_size
                center = self.center
        else:
            box_size = int(box_size)

        r_l = int(center[0] - box_size / 2.0)
        r_r = int(r_l + box_size)
        r_t = int(center[1] - box_size / 2.0)
        r_b = int(r_t + box_size)

        raw_l = 0 if r_l < 0 else r_l
        raw_t = 0 if r_t < 0 else r_t
        raw_r = img_width if r_r > img_width else r_r
        raw_b = img_height if r_b > img_height else r_b

        self.offset = [r_l, r_t]

        if not self.valid_beliefs(points_belief):
            # tracking failed
            self.bbx = [0, 0, self.wndWidth, self.wndHeight]
            self.frame_num = 0
            self.center = np.asarray([self.wndWidth / 2.0, self.wndHeight / 2.0])
            self.box_size = max([self.wndWidth, self.wndHeight])
            self.scale = float(self.box_size) / self.crop_box_size
            self.offset = [(self.wndWidth - self.box_size) / 2.0, (self.wndHeight - self.box_size) / 2.0]
            return img
        else:
            if self.frame_num < 2*self.initial_frame_num:
                self.frame_num += 1

            result = cv.copyMakeBorder(img[raw_t:raw_b, raw_l:raw_r], top=raw_t - r_t, bottom=r_b - raw_b, left=raw_l - r_l, right=r_r - raw_r, borderType=cv.BORDER_CONSTANT, value=[128, 128, 128])
            self.scale = float(result.shape[0]) / self.crop_box_size
            self.bbx = [raw_l, raw_t, raw_r, raw_b]
            self.box_size = box_size
            self.center = center
            return result

    def valid_beliefs(self, belief):
        if self.frame_num < self.initial_frame_num:
            return True

        total_num = float(len(belief))
        valid_num = np.sum(np.uint8(belief > 0.15))
        # If the belief of half points is below 0.2, then loss tracking
        if valid_num / total_num < 0.65:
            return False
        else:
            return True
    def draw_box(self, img):
        cv.line(img, (int(self.bbx[0]), int(self.bbx[1])), (int(self.bbx[0]), int(self.bbx[3])), (255, 255, 0), 3)
        cv.line(img, (int(self.bbx[0]), int(self.bbx[1])), (int(self.bbx[2]), int(self.bbx[1])), (255, 255, 0), 3)
        cv.line(img, (int(self.bbx[2]), int(self.bbx[1])), (int(self.bbx[2]), int(self.bbx[3])), (255, 255, 0), 3)
        cv.line(img, (int(self.bbx[2]), int(self.bbx[3])), (int(self.bbx[0]), int(self.bbx[3])), (255, 255, 0), 3)
        return img
