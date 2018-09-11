#ifndef M_RENDER_PARAMETER
#define M_RENDER_PARAMETER

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <string>
#include <QString>

/************************** Attention! ****************************/
/****** If you change to use the AR mode or not, you need to ******/
/****** changed the mLightPos and the bias in multilight.frag *****/
/****** also you need to changed the farplane                 *****/
/******************************************************************/

namespace mRenderParams {
static int cropTargetSize = 368;
static glm::vec3 mBgColor(0.2, 0.2, 0.2);
/********************* Window parameters ***********************/
static int mWindowWidth = 1024;
static int mWindowHeight = 1024;
/******************************************************************/

/******************** Mocap Data parameters ***********************/

/*** Normal Mode ***/
static int m_camera_type = 0; // 0 means perspective, 1 means 'ortho'
static glm::vec4 m_initial_proj_vec = glm::vec4(glm::radians(45.f), (float)mWindowWidth / mWindowHeight, 0.01f, 1000000.f);
static glm::mat4 m_initial_ortho_mat = glm::transpose(glm::ortho(-mWindowWidth / 16.0f, mWindowWidth / 16.0f, -mWindowHeight / 16.0f, mWindowHeight / 16.0f, -500000.f, 500000.f));
static glm::mat4 m_initial_view_mat = glm::transpose(glm::lookAt(glm::vec3(0, 100.f, 300.f), glm::vec3(0, 100.f, 0), glm::vec3(0, 1, 0)));
/*******************/

/******************************************************************/

/************************Shader parameters*************************/
static int mLightSum = 2; /****** Remember to modify the light_num in multilight.frag shader file ******/

static std::vector<glm::vec3> mLightPos({glm::vec3(300, 300, 300), glm::vec3(-300, 300, -300)});
static glm::vec3 mAmbient(0.3f, 0.3f, 0.3f);
static glm::vec3 mDiffuse(0.5f, 0.5f, 0.5f);
static glm::vec3 mSpecular(0.4f, 0.4f, 0.4f);

/******************** Light Cube parameter ************************/
static int mShadowUseShadow = 1;
static int mShadowWndWidth = 4096;
static int mShadowWndHeight = 4096;

static float mBias = 1;

static float mShadowNearPlane = 1.0f;
static float mShadowFarPlane  = 10000.0f; // TODO This plane must cover all the models you will render

static glm::mat4 mShadowProj = glm::perspective(glm::radians(90.0f), (float)mShadowWndWidth / (float)mShadowWndHeight, mShadowNearPlane, mShadowFarPlane);

// light 1 cube
static std::vector<std::vector<glm::mat4>> mShadowTransforms({
std::vector<glm::mat4>({
    mShadowProj * glm::lookAt(mLightPos[0], mLightPos[0] + glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
    mShadowProj * glm::lookAt(mLightPos[0], mLightPos[0] + glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
    mShadowProj * glm::lookAt(mLightPos[0], mLightPos[0] + glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
    mShadowProj * glm::lookAt(mLightPos[0], mLightPos[0] + glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
    mShadowProj * glm::lookAt(mLightPos[0], mLightPos[0] + glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
    mShadowProj * glm::lookAt(mLightPos[0], mLightPos[0] + glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
}),
std::vector<glm::mat4>({
    mShadowProj * glm::lookAt(mLightPos[1], mLightPos[1] + glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
    mShadowProj * glm::lookAt(mLightPos[1], mLightPos[1] + glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
    mShadowProj * glm::lookAt(mLightPos[1], mLightPos[1] + glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
    mShadowProj * glm::lookAt(mLightPos[1], mLightPos[1] + glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
    mShadowProj * glm::lookAt(mLightPos[1], mLightPos[1] + glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
    mShadowProj * glm::lookAt(mLightPos[1], mLightPos[1] + glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
})
});

/******************************************************************/

/************************Shader file path**************************/
static std::vector<QString> mDepthShaderFiles({":/shaders/depth.vert",
                                              ":/shaders/depth.frag",
                                              ":/shaders/depth.geo"});

static std::vector<QString > mPoseShaderFiles({":/shaders/scene.vert",
                                               ":/shaders/scene.frag"});

/******************************************************************/


/***************************** Model Render Colors ************************************/
static float skeleton_style[] = {
    1, 1.0, // head
    1, 1.0, // left shoulder
    1, 1.0, // left upper arm
    1, 1.0, // left lower arm
    1, 1.0, // right shoulder
    1, 1.0, // right upper arm
    1, 1.0, // right lower arm
    1, 1.0, // spine
    1, 1.0, // left hip
    1, 1.0, // left ham,
    1, 1.0, // left calf,
    1, 1.0, // right hip
    1, 1.0, // right ham,
    1, 1.0, // right calf,
    1, 1.0, // left feet,
    1, 1.0, // right feet
};

static glm::vec3 mBoneColors[] = {
    glm::vec3(1.000000, 1.000000, 0.000000),
    glm::vec3(0.492543, 0.000000, 0.000000),
    glm::vec3(0.000000, 1.000000, 0.000000),
    glm::vec3(1.000000, 0.349454, 0.000000),
    glm::vec3(0.499439, 0.558884, 1.000000),
    glm::vec3(0.000000, 0.362774, 0.000000),
    glm::vec3(0.500312, 0.000000, 0.624406),
    glm::vec3(0.501744, 0.724322, 0.275356),
    glm::vec3(0.000000, 1.000000, 1.000000),
    glm::vec3(1.000000, 0.000000, 1.000000),
    glm::vec3(1.000000, 0.499433, 0.611793),
    glm::vec3(1.000000, 0.800000, 1.000000),
    glm::vec3(0.000000, 0.502502, 0.611632),
    glm::vec3(0.000000, 0.000000, 1.000000)
};

static std::vector<glm::vec3> mJointColors({
    glm::vec3(1.000000, 1.000000, 0.000000), // head
    glm::vec3(0.501744, 0.724322, 0.275356), // neck

    glm::vec3(0.492543, 0.000000, 0.000000), // left shoulder
    glm::vec3(0.000000, 1.000000, 0.000000), // left upper arm
    glm::vec3(1.000000, 0.349454, 0.000000), // left lower arm
    glm::vec3(0.499439, 0.558884, 1.000000), // right shoulder
    glm::vec3(0.000000, 0.362774, 0.000000), // right upper arm
    glm::vec3(0.500312, 0.000000, 0.624406), // right lower arm

    glm::vec3(0.000000, 1.000000, 1.000000),// left hip
    glm::vec3(1.000000, 0.000000, 1.000000),// left ham,
    glm::vec3(1.000000, 0.499433, 0.611793),// left calf,
    glm::vec3(1.000000, 0.800000, 1.000000),// right hip
    glm::vec3(0.000000, 0.502502, 0.611632),// right ham,
    glm::vec3(0.000000, 0.000000, 1.000000), // right calf,

    glm::vec3(0.501744, 0.724322, 0.275356), // root ,
});

}
#endif
