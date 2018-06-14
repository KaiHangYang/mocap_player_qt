#ifndef SCENE_UTILS
#define SCENE_UTILS

#include <glm/glm.hpp>
#include <QtOpenGL>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions_3_3_Core>

#include "mPoseModel.h"
#include "mShader.h"

#include <opencv2/core/core.hpp>

class mSceneUtils {
public:
    mSceneUtils(QOpenGLVertexArrayObject * vao, QOpenGLFunctions_3_3_Core * core_func, int wnd_width, int wnd_height, glm::mat4 cam_in_mat, glm::mat4 cam_ex_mat, bool is_ar=false, int pose_type=0);
    ~mSceneUtils();

    void render(std::vector<glm::vec3> points_3d = std::vector<glm::vec3>(0), glm::mat4 cam_ex_mat = glm::mat4(0.f));
    void render(std::vector<glm::vec3> points_3d = std::vector<glm::vec3>(0), glm::vec3 cam_ex_vec = glm::vec3(0.f));

    void setExMat(glm::mat4 & cam_ex_mat);
    void setInMat(glm::mat4 & cam_in_mat);

    void setCurExMat(glm::mat4 cur_ex_mat);

    void getCurExMat(glm::mat4 & cam_ex_r_mat, glm::mat4 & cam_ex_t_mat);
    glm::mat4 getCurExMat();
    glm::mat4 getRawExMat();
    glm::vec3 getCurFollowVec();
    void setCurFollowVec(glm::vec3 cur_follow_vec);
    void getSplittedCameras(int camera_num, std::vector<glm::vec3> &splitted_cameras);
    void getSplittedCameras(int camera_num, std::vector<glm::mat4> &splitted_cameras);
    void setFollowDefault();

    void getLabelsFromFrame(const std::vector<glm::vec3> &joints, const glm::mat4 &view_mat, std::vector<glm::vec2> &labels_2d, std::vector<glm::vec3> &labels_3d);
    void getLabelsFromFrame(const std::vector<glm::vec3> &joints, const glm::vec3 &view_vec, std::vector<glm::vec2> &labels_2d, std::vector<glm::vec3> &labels_3d);

    void moveCamera(int move_type, QMouseEvent * event = NULL);
    void rotateCamrea(const glm::mat4 & rotate_mat);
    void setFloor(bool is_with_floor=true);

    void setFollowPerson(bool is_follow);
    void setFocusOnCenter(bool is_focus_on_center);
    bool getFollowPerson();
    bool getFocusOnCenter();
    void captureFrame(cv::Mat & cur_frame);


    // Just for tmp adjust for normal and vr mode
    float m_move_dir[3];
    float m_rotate_dir[2];

private:
    std::vector<GLfloat> getGroundVertexs();
    std::vector<GLfloat> getGroundColor();

    void _beforeRender(const std::vector<glm::vec3> & points_3d);
    void _render(std::vector<glm::vec3> points_3d, glm::mat4 cur_cam_ex_mat);
    void _getLabelsFromFrame(const std::vector<glm::vec3> & joints, const glm::mat4 & view_mat, std::vector<glm::vec2> & labels_2d, std::vector<glm::vec3> & labels_3d);
    glm::mat4 convertVec2Mat(const glm::vec3 & follow_vec, glm::vec3 person_center_pos);

    int wnd_width;
    int wnd_height;
    int ground_col;
    int ground_row;
    int array_size;
    float ground_size;
    float move_step_scale;
    bool is_follow_person;
    bool is_focus_on_center;
    bool is_with_floor;

    QOpenGLVertexArrayObject * VAO;

    GLuint ground_vbo;
    GLuint ground_cbo;
    GLuint ground_tbo;

    std::vector<GLuint> shadow_fbo;
    std::vector<GLuint> shadow_tbo;

    QOpenGLFunctions_3_3_Core * core_func;

    glm::mat4 cam_proj_mat;
    glm::mat4 cam_ex_mat;
    glm::mat4 cam_ex_mat_inverse;
    glm::mat4 cam_in_mat;
    glm::mat4 rotate_mat;
    glm::mat4 cam_ex_t_mat;
    glm::mat4 cam_ex_r_mat;

    glm::mat4 cur_cam_ex_r_mat;
    glm::mat4 cur_cam_ex_t_mat;

    glm::vec3 cur_follow_dert;
    glm::vec3 person_center_pos;

    glm::vec2 prev_mouse_pos;

    mShader * scene_shader;
    mShader * depth_shader;

    mPoseModel * pose_model;
    bool is_ar;




};

#endif
