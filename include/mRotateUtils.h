#ifndef M_ROTATE_UTILS
#define M_ROTATE_UTILS

#include <glm/glm.hpp>
#include <QMouseEvent>

namespace mArcBall {

    void mouse_button_callback(QMouseEvent * event);
    void mouse_move_callback(QMouseEvent * event);
//    glm::vec3 getArcballVector(float x, float y, int wnd_width, int wnd_height);
    glm::mat4 getRotateMat(int wnd_width, int wnd_height, glm::mat4 &view_mat);
    void resetRotateMat();
}

namespace mCamRotate {

    void mouse_button_callback(QMouseEvent * event);
    void mouse_move_callback(QMouseEvent * event);
    glm::mat4 getRotateMat(int wnd_width, int wnd_height, const glm::mat4 & view_r_mat, float rotate_dir[2]);
}
#endif
