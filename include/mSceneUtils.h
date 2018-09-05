#ifndef SCENE_UTILS
#define SCENE_UTILS

#include <glm/glm.hpp>
#include <QtOpenGL>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions_3_3_Core>

#include "mPoseModel.h"
#include "mShader.h"
#include "mCamera.h"
#include "mMeshReader.h"

#include <opencv2/core/core.hpp>

class mSceneUtils {
public:
    mSceneUtils(QOpenGLVertexArrayObject * vao, QOpenGLFunctions_3_3_Core * core_func, int wnd_width, int wnd_height, glm::mat4 cam_in_mat, glm::mat4 cam_ex_mat, int camera_type, bool & is_with_floor, bool & use_shading, bool is_ar=false, int pose_type=0);
    ~mSceneUtils();

    void render(std::vector<glm::vec3> points_3d_raw, std::vector<glm::vec3> points_3d = std::vector<glm::vec3>(0), const mCamera * camera=nullptr);
    void renderCamerasPos(std::vector<const mCamera *> cameras);

    int getCurCameraType();
    void setCurCameraType(int camera_type);

    const mCamera * getCurCamera();
    void setCurCamera(const mCamera * camera);

    void setCurExMat(glm::mat4 cur_ex_mat);
    glm::mat4 getCurExMat();
    glm::mat4 getCurExMat(glm::mat4 & cam_ex_r_mat, glm::mat4 & cam_ex_t_mat);

    void setCurFollowVec(glm::vec3 cur_follow_vec);
    glm::vec3 getCurFollowVec();

    void setCurInMat(glm::mat4 proj_mat);

    void getSplittedCameras(int camera_num, std::vector<glm::vec3> &splitted_cameras);
    void getSplittedCameras(int camera_num, std::vector<glm::mat4> &splitted_cameras);

    void getLabelsFromFrame(const std::vector<glm::vec3> & joints_raw, const std::vector<glm::vec3> & joints_adjusted, const mCamera * camera, std::vector<glm::vec2> &labels_2d, std::vector<glm::vec3> &labels_3d);

    void get2DJointsOnCurCamera(const std::vector<glm::vec3> & joints_3d, std::vector<glm::vec2> & joints_2d);

    void moveCamera(int move_type, QMouseEvent * event = NULL);
    void rotateCamera(glm::mat4 rotate_mat);

    void setFollowPerson(bool is_follow);
    void setFocusOnCenter(bool is_focus_on_center);
    bool getFollowPerson();
    bool getFocusOnCenter();
    void captureFrame(cv::Mat & cur_frame);

    void setVerticalAngle(float angle);
    void setPoseCenter(glm::vec3 pose_center);

    std::vector<glm::vec3> adjustPoseAccordingToCamera(std::vector<glm::vec3> joints_3d, const mCamera * camera = nullptr);

    // Just for tmp adjust for normal and vr mode
    float m_move_dir[3];
    float m_rotate_dir[2];

private:
    void initScene();
    std::vector<GLfloat> getGroundVertexs();
    std::vector<GLfloat> getGroundColor();

    void _beforeRender(const std::vector<glm::vec3> & points_3d);
    void _render(std::vector<glm::vec3> points_3d_raw, std::vector<glm::vec3> points_3d, glm::mat4 cur_cam_ex_mat, glm::mat4 cur_cam_in_mat, int camera_type);
    void _setDepthShaderUniforms(int light_num);
    void _setSceneShaderUnoforms(glm::mat4 model_mat, glm::mat4 view_mat, glm::mat4 proj_mat, bool is_use_shadow);
    void _drawFloor();
    void _getLabelsFromFrame(const std::vector<glm::vec3> & joints_raw, const std::vector<glm::vec3> & joints_adjusted, const glm::mat4 & view_mat, const glm::mat4 & proj_mat, std::vector<glm::vec2> & labels_2d, std::vector<glm::vec3> & labels_3d);


    int wnd_width;
    int wnd_height;
    int ground_col;
    int ground_row;
    int array_size;
    float ground_size;
    float move_step_scale;

    QOpenGLVertexArrayObject * VAO;

    GLuint ground_vbo;
    GLuint ground_cbo;
    GLuint ground_tbo;

    std::vector<GLuint> shadow_fbo;
    std::vector<GLuint> shadow_tbo;

    QOpenGLFunctions_3_3_Core * core_func;

    glm::mat4 rotate_mat;
    glm::vec3 person_center_pos;

    glm::vec2 prev_mouse_pos;

    mShader * scene_shader;
    mShader * depth_shader;

    mPoseModel * pose_model;
    mCamera * cur_camera; // The inner camera
    mMeshReader * camera_mesh;

    bool is_ar;
    /********************** State parameter need to share **********************/
    bool & is_with_floor;
    bool & use_shading;
};

#endif
