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

    void render(std::vector<float> points_3d = std::vector<float>(0, 0));

    void setExMat(glm::mat4 & cam_ex_mat);
    void setInMat(glm::mat4 & cam_in_mat);

    void setCurExMat(glm::mat4 cur_ex_mat);

    void getCurExMat(glm::mat4 & cam_ex_r_mat, glm::mat4 & cam_ex_t_mat);
    glm::mat4 getCurExMat();
    glm::mat4 getRawExMat();

    void moveCamera(int move_dir);
    void rotateCamrea(const glm::mat4 & rotate_mat);

    void setFollowPerson(bool is_follow);
    void captureFrame(cv::Mat & cur_frame);

    void setSurround(bool do_surround, glm::vec3 surround_center = glm::vec3(0.0,0.0,0.0));
private:
    std::vector<GLfloat> getGroundVertexs();
    std::vector<GLfloat> getGroundColor();
    void surroundOnePoint(glm::mat4 & model_mat);

    int wnd_width;
    int wnd_height;
    int ground_col;
    int ground_row;
    int array_size;
    float ground_size;
    float move_step_scale;
    bool is_follow_person;

    QOpenGLVertexArrayObject * VAO;

    GLuint ground_vbo;
    GLuint ground_cbo;
    GLuint ground_tbo;

    GLuint shadow_fbo;
    GLuint shadow_tbo;

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

    glm::vec2 cur_follow_dert;
    glm::vec3 person_center_pos;

    /***** center surround *****/
    glm::vec3 surround_center;
    bool do_use_surround;
    /***************************/

    mShader * scene_shader;
    mShader * depth_shader;

    mPoseModel * pose_model;
    bool is_ar;

};

#endif
