#-------------------------------------------------
#
# Project created by QtCreator 2018-04-09T10:18:00
#
#-------------------------------------------------

QT       += core gui
QT       += opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = opengl_demo
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
    src/gl_utils/mShader.cpp \
    main.cpp \
    mGLWidget.cpp \
    mMainWindow.cpp \
    src/gl_utils/mRotateUtils.cpp

HEADERS += \
    include/bvh-parser/bvh-parser.h \
    include/bvh-parser/bvh.h \
    include/bvh-parser/joint.h \
    include/bvh-parser/utils.h \
    include/mMeshReader.h \
    include/mShader.h \
    mGLWidget.h \
    mMainWindow.h \
    include/mRotateUtils.h

INCLUDEPATH += ./include/
LIBS += -lassimp

FORMS += \
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
