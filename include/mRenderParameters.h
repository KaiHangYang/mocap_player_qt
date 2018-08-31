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

/********************* Window parameters ***********************/
//static int mWindowWidth = 2048;
//static int mWindowHeight = 2048;
static int mWindowWidth = 1024;
static int mWindowHeight = 1024;
/******************************************************************/


/******************** Mocap Data parameters ***********************/
//#define M_AR_MODE

/*** AR MODE ***/
//#ifdef M_AR_MODE
//static bool m_is_ar = true;
//static glm::mat4 m_cam_in_mat = glm::mat4({1500.172, 0, mWindowWidth / 2.f, 0, 0, 1500.837, mWindowHeight / 2.f, 0, 0, 0, 1, 0, 0, 0, 0, 1});
//static glm::mat4 m_cam_ex_mat = glm::mat4({0.000575281, 0.06160985, -0.9981001, 221.3543, 0.2082146, -0.9762325, -0.06013997, 659.87, -0.978083, -0.2077844, -0.01338968, 3644.688, 0,
//                              0, 0, 1});
//// camera 0
//static glm::mat4 m_cam_in_mat = glm::mat4({1497.693, 0, 1024.704, 0, 0, 1497.103, 1051.394, 0, 0, 0, 1, 0, 0, 0, 0, 1});
//static glm::mat4 m_cam_ex_mat = glm::mat4({0.9650164, 0.00488022, 0.262144, -562.8666, -0.004488356, -0.9993728, 0.0351275, 1398.138, 0.262151, -0.03507521, -0.9643893, 3852.623, 0, 0, 0, 1});
//// camera 1
//static glm::mat4 m_cam_in_mat = glm::mat4({1495.217, 0, 1030.519, 0, 0, 1495.52, 1052.626, 0, 0, 0, 1, 0, 0, 0, 0, 1});
//static glm::mat4 m_cam_ex_mat = glm::mat4({0.6050639, -0.02184232, 0.7958773, -1429.856, -0.22647, -0.9630526, 0.1457429, 738.1779, 0.7632883, -0.2684261, -0.587655, 4897.966, 0, 0, 0, 1});
//// camera 2
//static glm::mat4 m_cam_in_mat = glm::mat4({1495.587, 0, 983.8873, 0, 0, 1497.828, 987.5902, 0, 0, 0, 1, 0, 0, 0, 0, 1});
//static glm::mat4 m_cam_ex_mat = glm::mat4({-0.3608179, -0.009492658, 0.932588, 57.25702, -0.0585942, -0.9977421, -0.03282591, 1307.287, 0.9307939, -0.06648842, 0.359447, 2799.822, 0, 0, 0, 1});
//// camera 3
//static glm::mat4 m_cam_in_mat = glm::mat4({1495.886, 0, 1029.06, 0, 0, 1496.033, 1041.409, 0, 0, 0, 1, 0, 0, 0, 0, 1});
//static glm::mat4 m_cam_ex_mat = glm::mat4({-0.0721105, -0.04817664, 0.9962325, -284.8168, -0.4393254, -0.8951841, -0.07508985, 807.9184, 0.895429, -0.443085, 0.04338695, 3177.16, 0, 0, 0, 1});
//// camera 4
//static glm::mat4 m_cam_in_mat = glm::mat4({1490.952, 0, 987.6075, 0, 0, 1491.108, 1019.069, 0, 0, 0, 1, 0, 0, 0, 0, 1});
//static glm::mat4 m_cam_ex_mat = glm::mat4({0.3737275, 0.09688602, 0.9224646, -1563.911, -0.009716132, -0.9940662, 0.1083427, 801.9608, 0.9274878, -0.04945343, -0.3705685, 3517.316, 0, 0, 0, 1});
//// camera 7
//static glm::mat4 m_cam_in_mat = glm::mat4({1498.831, 0, 987.2716, 0, 0, 1499.674, 976.8773, 0, 0, 0, 1, 0, 0, 0, 0, 1});
//static glm::mat4 m_cam_ex_mat = glm::mat4({-0.9995936, 0.02847456, 0.001368653, 1378.866, -0.02843213, -0.9992908, 0.0246889, 1270.781, 0.002070688, 0.02463995, 0.9996943, 2631.567, 0, 0, 0, 1});
//// camera 8
//static glm::mat4 m_cam_in_mat = glm::mat4({1500.172, 0, 1017.387, 0, 0, 1500.837, 1043.032, 0, 0, 0, 1, 0, 0, 0, 0, 1});
//static glm::mat4 m_cam_ex_mat = glm::mat4({0.000575281, 0.06160985, -0.9981001, 221.3543, 0.2082146, -0.9762325, -0.06013997, 659.87, -0.978083, -0.2077844, -0.01338968, 3644.688, 0, 0, 0, 1});
//// camera 9
//static glm::mat4 m_cam_in_mat = glm::mat4({1501.554, 0, 1010.423, 0, 0, 1501.9, 1037.096, 0, 0, 0, 1, 0, 0, 0, 0, 1});
//static glm::mat4 m_cam_ex_mat = glm::mat4({0.04176839, 0.00780962, -0.9990969, 388.6217, 0.5555364, -0.831324, 0.01672664, 137.5452, -0.8304425, -0.5557333, -0.03906159, 4216.635, 0, 0, 0, 1});
/***************/
//#else
/*** Normal Mode ***/
static bool m_is_ar = false;
static int m_camera_type = 0; // 0 means perspective, 1 means 'ortho'
static glm::mat4 m_cam_in_mat_perspective = glm::transpose(glm::perspective(glm::radians(45.f), (float)mWindowWidth / mWindowHeight, 0.01f, 1000000.f));
static glm::mat4 m_cam_in_mat_ortho = glm::transpose(glm::ortho(-mWindowWidth / 16.0f, mWindowWidth / 16.0f, -mWindowHeight / 16.0f, mWindowHeight / 16.0f, -500000.f, 500000.f));
static glm::mat4 m_cam_ex_mat = glm::transpose(glm::lookAt(glm::vec3(0, 10.f, 300.f), glm::vec3(0, 10.f, 0), glm::vec3(0, 1, 0)));
/*******************/
//#endif
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

//static glm::vec3 mBoneColors[] = {
//    glm::vec3(0, 0.45, 0), // head
//    glm::vec3(0.15, 0, 0), // left shoulder
//    glm::vec3(0.30, 0, 0), // left upper arm
//    glm::vec3(0.45, 0, 0), // left lower arm
//    glm::vec3(0, 0, 0.15), // right shoulder
//    glm::vec3(0, 0, 0.30), // right upper arm
//    glm::vec3(0, 0, 0.45), // right lower arm
//    glm::vec3(0, 0.90, 0), // spine
//    glm::vec3(0.60, 0, 0), // left hip
//    glm::vec3(0.75, 0, 0), // left ham,
//    glm::vec3(0.90, 0, 0), // left calf,
//    glm::vec3(0, 0, 0.60), // right hip
//    glm::vec3(0, 0, 0.75), // right ham,
//    glm::vec3(0, 0, 0.90), // right calf,
//    glm::vec3(1, 1, 1), // left feet,
//    glm::vec3(1, 1, 1) // right feet
//};

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
//static glm::vec3 mBoneColors[] = {
//    glm::vec3(0.9451, 0.9294, 0.0196), // head #f1ed05
//    glm::vec3(0.3098, 0.2392, 0.0902), // left shoulder #4f3d17
//    glm::vec3(0.0, 0.9137, 0.3451), // left upper arm #00e958
//    glm::vec3(0.6118, 0.0, 0.1725), // left lower arm #9c002c
//    glm::vec3(0.6431, 0.6353, 0.6392), // right shoulder #a4a2a3
//    glm::vec3(0.0, 0.4, 0.0902), // right upper arm #006617
//    glm::vec3(0.2824, 0.1255, 0.7373), // right lower arm #4820bc
//    glm::vec3(0.8078, 0.5922, 0.2471), // spine #ce973f
//    glm::vec3(0.0, 0.9608, 0.9961), // left hip #00f5fe
//    glm::vec3(1.0, 0.2078, 0.7843), // left ham #ff35c8
//    glm::vec3(1.0, 0.3922, 0.4039), // left calf #ff6467
//    glm::vec3(1.0, 0.8196, 0.8235), // right hip #ffd1d2
//    glm::vec3(0.0, 0.5922, 0.5647), // right ham #009790
//    glm::vec3(0.0, 0.4863, 0.9608), // right calf #007cf5
//};

static std::vector<glm::vec3> mJointColors({
    glm::vec3(0, 1, 0), // head
    glm::vec3(0, 1, 0), // neck

    glm::vec3(1, 0, 0), // left shoulder
    glm::vec3(1, 0, 0), // left upper arm
    glm::vec3(1, 0, 0), // left lower arm
    glm::vec3(0, 0.6, 1), // right shoulder
    glm::vec3(0, 0.6, 1), // right upper arm
    glm::vec3(0, 0.6, 1), // right lower arm

    glm::vec3(1, 0, 0), // left hip
    glm::vec3(1, 0, 0), // left ham,
    glm::vec3(1, 0, 0), // left calf,
    glm::vec3(0, 0.6, 1), // right hip
    glm::vec3(0, 0.6, 1), // right ham,
    glm::vec3(0, 0.6, 1), // right calf,

    glm::vec3(0, 1, 0), // root ,

    glm::vec3(1, 0, 0), // left feet,
    glm::vec3(0, 0.6, 1) // right feet
});

}
#endif
