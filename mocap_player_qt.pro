#-------------------------------------------------
#
# Project created by QtCreator 2018-04-09T10:18:00
#
#-------------------------------------------------

QT       += core gui
QT       += opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mocap_player_qt
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    src/bvh_parser/bvh-parser.cc \
    src/bvh_parser/bvh.cc \
    src/gl_utils/mMeshReader.cpp \
    src/gl_utils/mRotateUtils.cpp \
    src/gl_utils/mShader.cpp \
    src/ui_widgets/mGLWidget.cpp \
    src/ui_widgets/mMainWindow.cpp \
    src/scene_utils/mSceneUtils.cpp \
    src/pose_utils/mPoseModel.cpp \
    main.cpp \
    src/mocap_reader/mMoCapReader.cpp \
    src/ui_widgets/mProgressBarWidget.cpp \
    src/ui_widgets/mLineEditWidget.cpp \
    src/pose_utils/mPoseAdjuster.cpp \
    src/visual_utils/mVisual.cpp \
    src/optimize_utils/mOurOpt.cpp


HEADERS += \
    include/bvh-parser/bvh-parser.h \
    include/bvh-parser/bvh.h \
    include/bvh-parser/joint.h \
    include/bvh-parser/utils.h \
    include/mGLWidget.h \
    include/mMainWindow.h \
    include/mMeshReader.h \
    include/mPoseModel.h \
    include/mRenderParameters.h \
    include/mRotateUtils.h \
    include/mSceneUtils.h \
    include/mShader.h \
    include/mPoseDefs.h \
    include/mMoCapReader.h \
    include/mProgressBarWidget.h \
    include/mLineEditWidget.h \
    include/mPoseAdjuster.h \
    include/mVisual.h \
    include/mOurOpt.h


INCLUDEPATH += ./include/ \
            /usr/local/Cellar/glm/0.9.8.5/include \
            /usr/local/Cellar/assimp/3.2/include \
            /usr/local/Cellar/opencv/3.0.0/include \
            /usr/include/eigen3
LIBS += -L/usr/local/Cellar/assimp/3.2/lib/ -lassimp -L/usr/local/lib/ -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lceres -lglog

FORMS += \
        window.ui \
    window.ui

DISTFILES += \
    shaders/cam_f.shader \
    shaders/cam_v.shader \
    shaders/model_f.shader \
    shaders/model_v.shader \
    shaders/multilight_f.shader \
    shaders/multilight_v.shader \
    shaders/scene_f.shader \
    shaders/scene_v.shader \
    shaders/depth.geo \
    shaders/depth.frag \
    shaders/multilight.frag \
    shaders/depth.vert \
    shaders/multilight.vert

RESOURCES += \
    resources.qrc
