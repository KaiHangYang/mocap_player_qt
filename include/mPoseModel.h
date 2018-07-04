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

    mPoseModel(QOpenGLVertexArrayObject * vao, QOpenGLFunctions_3_3_Core * core_func, mShader * pose_shader, mShader * depth_shader, float target_model_size, bool is_ar, bool use_shading, int pose_type=0);
    ~mPoseModel();

    void draw(std::vector<glm::vec3> points, glm::mat4 & cam_ex_mat, int render_type=0);
    void setUseShading(bool use_shading);
    /***************Pose parameters********************/
    std::vector<glm::u32vec2> bone_indices;
    int num_of_joints;
private:

    void renderPose(std::vector<glm::vec3> &vertexs, glm::mat4 view_mat, int render_type = 0);

    mMeshReader * mesh_reader;

    float model_size;
    float model_scale;

    mShader * depth_shader;
    mShader * pose_shader;

    bool is_ar;
    bool use_shading;
    QOpenGLVertexArrayObject * VAO;
    QOpenGLFunctions_3_3_Core * core_func;
};

#endif
