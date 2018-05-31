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

/************************Shader parameters*************************/
static int mLightSum = 2; /****** Remember to modify the light_num in multilight.frag shader file ******/

static std::vector<glm::vec3> mLightPos({glm::vec3(300, 300, 300), glm::vec3(-300, 300, -300)});

static std::vector<glm::vec3> mLightPos_AR({glm::vec3(3000, 3000, 3000), glm::vec3(-3000, 3000, -3000)}); // currently not modified

static glm::vec3 mAmbient(0.3f, 0.3f, 0.3f);
static glm::vec3 mDiffuse(0.5f, 0.5f, 0.5f);
static glm::vec3 mSpecular(0.4f, 0.4f, 0.4f);

/******************** Light Cube parameter ************************/
static int mShadowUseShadow = 1;
static int mShadowWndWidth = 4096;
static int mShadowWndHeight = 4096;

static float mBias = 1;
static float mBias_AR = 300;

static float mShadowNearPlane = 1.0f;
static float mShadowFarPlane  = 10000.0f; // TODO This plane must cover all the models you will render
static float mShadowFarPlane_AR  = 1000000.0f; // TODO This plane must cover all the models you will render

static glm::mat4 mShadowProj = glm::perspective(glm::radians(90.0f), (float)mShadowWndWidth / (float)mShadowWndHeight, mShadowNearPlane, mShadowFarPlane);
static glm::mat4 mShadowProj_AR = glm::perspective(glm::radians(90.0f), (float)mShadowWndWidth / (float)mShadowWndHeight, mShadowNearPlane, mShadowFarPlane_AR);

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

static std::vector<std::vector<glm::mat4>> mShadowTransforms_AR({
std::vector<glm::mat4>({
    mShadowProj_AR * glm::lookAt(mLightPos_AR[0], mLightPos_AR[0] + glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
    mShadowProj_AR * glm::lookAt(mLightPos_AR[0], mLightPos_AR[0] + glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
    mShadowProj_AR * glm::lookAt(mLightPos_AR[0], mLightPos_AR[0] + glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
    mShadowProj_AR * glm::lookAt(mLightPos_AR[0], mLightPos_AR[0] + glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
    mShadowProj_AR * glm::lookAt(mLightPos_AR[0], mLightPos_AR[0] + glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
    mShadowProj_AR * glm::lookAt(mLightPos_AR[0], mLightPos_AR[0] + glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
}),
std::vector<glm::mat4>({
    mShadowProj_AR * glm::lookAt(mLightPos_AR[1], mLightPos_AR[1] + glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
    mShadowProj_AR * glm::lookAt(mLightPos_AR[1], mLightPos_AR[1] + glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
    mShadowProj_AR * glm::lookAt(mLightPos_AR[1], mLightPos_AR[1] + glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
    mShadowProj_AR * glm::lookAt(mLightPos_AR[1], mLightPos_AR[1] + glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
    mShadowProj_AR * glm::lookAt(mLightPos_AR[1], mLightPos_AR[1] + glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
    mShadowProj_AR * glm::lookAt(mLightPos_AR[1], mLightPos_AR[1] + glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
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

#endif
