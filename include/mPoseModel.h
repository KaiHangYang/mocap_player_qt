#ifndef M_POSEMODEL
#define M_POSEMODEL

#include <glm/glm.hpp>
#include <string>
#include <QtOpenGL>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions_3_3_Core>

#include "mShader.h"
#include "mMeshReader.h"

class mPoseModel {
public:

    mPoseModel(QOpenGLVertexArrayObject * vao, QOpenGLFunctions_3_3_Core * core_func, mShader * pose_shader, mShader * depth_shader, glm::mat4 cam_in_mat, float target_model_size, bool is_ar, int pose_type=0);
    ~mPoseModel();

    void draw(std::vector<float> points, glm::mat4 raw_cam_ex_mat_inverse, glm::mat4 & cam_ex_mat, int render_type=0);

    /***************Pose parameters********************/
    std::vector<unsigned int> bone_indices;
    int num_of_joints;
private:
    void renderPose(std::vector<float> &vertexs, glm::mat4 view_mat, int render_type = 0);

    mMeshReader *mesh_reader;

    float model_size;
    float model_scale;

    mShader * depth_shader;
    mShader * pose_shader;

    glm::mat4 proj_mat;

    bool is_ar;
    QOpenGLVertexArrayObject * VAO;
    QOpenGLFunctions_3_3_Core * core_func;
};

#endif
