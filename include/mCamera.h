#ifndef M_CAMERA
#define M_CAMERA
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <QMouseEvent>


class mCamera {
public:
    mCamera(glm::mat4 proj_mat, glm::mat4 view_mat, int wnd_width, int wnd_height, int id, bool is_ar = false);
    ~mCamera();

    glm::mat4 getProjMat();
    glm::mat4 getViewMat(glm::vec3 pose_center);
    glm::mat4 getViewMat(glm::vec3 pose_center, glm::mat4 &view_r_mat, glm::mat4 &view_t_mat);
    glm::vec3 getViewVec(glm::vec3 pose_center);

    bool getFollow();
    bool getFocus();

    void setViewMat(glm::mat4 view_mat);
    void setViewVec(glm::vec3 view_vec);

    void setFocus(bool is_focus);
    void setFollow(bool is_follow, glm::vec3 pose_center);
    void moveCamera(glm::vec3 move_step, glm::vec3 pose_center);
    void rotateCamera(glm::mat4 rotate_mat);

    /**** function used in different mode ****/

    void setVerticalAngle(float angle, glm::vec3 pose_center);
    void getSplittedCameras(int camera_num, glm::vec3 pose_center, std::vector<glm::mat4> &splitted_cameras);
    void getSplittedCameras(int camera_num, glm::vec3 pose_center, std::vector<glm::vec3> &splitted_cameras);



private:
    glm::vec3 follow_dert;
    bool is_follow;
    bool is_focus;

    int id;
    bool is_ar;
    glm::mat4 proj_mat;

    int wnd_width;
    int wnd_height;

    glm::mat4 view_r_mat;
    glm::mat4 view_t_mat;
};

#endif
