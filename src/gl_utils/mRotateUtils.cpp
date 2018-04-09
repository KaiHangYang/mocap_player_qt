#include "mRotateUtils.h"

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <QDebug>

namespace mArcBall {
    glm::mat4 rotate_mat(1.f);
    glm::vec2 init_pos(0.f);
    glm::vec2 cur_pos(0.f);
    void mouse_button_callback(QMouseEvent * event) {
        init_pos = glm::vec2(event->pos().x(), event->pos().y());
        cur_pos = init_pos;
    }
    
    void mouse_move_callback(QMouseEvent * event) {
        if (event->buttons() & Qt::LeftButton) {
            cur_pos = glm::vec2(event->pos().x(), event->pos().y());
        }
    }

    glm::vec3 getArcballVector(float x, float y, int wnd_width, int wnd_height) {
        glm::vec3 P = glm::vec3(1.0*x/wnd_width*2 - 1.0,
			  1.0*y/wnd_height*2 - 1.0,
			  0);
        P.y = -P.y;
        float OP_squared = P.x * P.x + P.y * P.y;
        if (OP_squared <= 1*1)
            P.z = sqrt(1*1 - OP_squared);  // Pythagore
        else
            P = glm::normalize(P);  // nearest point
        return P;
    }

    // set the camera rotate
    glm::mat4 getRotateMat(int wnd_width, int wnd_height, glm::mat4 &view_mat) {
        glm::mat4 mat_in_obj_coord = glm::mat4(1.f);
        if (init_pos != cur_pos) {
            // -1 is for rotate the camera
            glm::vec3 va = getArcballVector(init_pos.x, init_pos.y, wnd_width, wnd_height);
            glm::vec3 vb = getArcballVector(cur_pos.x, cur_pos.y, wnd_width, wnd_height);

            float angle = acos(fmin(1.0f, glm::dot(va, vb)));
            glm::vec3 axis_in_camera_coord = glm::cross(va, vb);
            glm::vec3 axis_in_obj_coord = glm::inverse(glm::mat3(view_mat)) * axis_in_camera_coord;

            mat_in_obj_coord = glm::rotate(mat_in_obj_coord, angle, axis_in_obj_coord);

            init_pos = cur_pos;
            rotate_mat = mat_in_obj_coord * rotate_mat;
        }
        return rotate_mat;
    }
    void resetRotateMat() {
        rotate_mat = glm::mat4(1.f);
    }
}


namespace mCamRotate {
    glm::vec2 init_pos(0.f);
    glm::vec2 cur_pos(0.f);
    void mouse_button_callback(QMouseEvent * event) {
        init_pos = glm::vec2(event->pos().x(), event->pos().y());
        cur_pos = init_pos;
    }
    
    void mouse_move_callback(QMouseEvent * event) {
        if (event->buttons() & Qt::LeftButton) {
            cur_pos = glm::vec2(event->pos().x(), event->pos().y());
        }
    }

    glm::mat4 getRotateMat(int wnd_width, int wnd_height, const glm::mat4 & view_r_mat) {

        if (init_pos != cur_pos) {

            glm::vec3 dir_z(view_r_mat[0][2], view_r_mat[1][2], view_r_mat[2][2]);
            glm::vec3 dir_y(0, 1, 0);

            glm::vec3 dir_x = glm::normalize(glm::cross(dir_y, dir_z));

            float div_x = cur_pos.x - init_pos.x;
            float div_y =  cur_pos.y - init_pos.y;

            float x_angle = div_x / wnd_width * 3.141592653589;
            float y_angle = div_y / wnd_height * 3.141592653589;

            glm::mat4 result_mat_y = glm::rotate(glm::mat4(1.f), x_angle, dir_y);
            glm::mat4 result_mat_x = glm::rotate(glm::mat4(1.f), y_angle, dir_x);

            init_pos = cur_pos;

            return result_mat_y * result_mat_x;
        }
        return glm::mat4(1.f);
    }
}
