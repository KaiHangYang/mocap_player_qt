#include "mGLWidget.h"
#include <QtWidgets>
#include <QDebug>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "mRotateUtils.h"

mGLWidget::mGLWidget(QWidget * parent, int wnd_width, int wnd_height) : QGLWidget(QGLFormat(QGL::SampleBuffers), parent) {
    this->wnd_width = wnd_width;
    this->wnd_height = wnd_height;

    this->timer_for_update = new QTimer(this);
    connect(timer_for_update, SIGNAL(timeout()), this, SLOT(update()));
}

mGLWidget::~mGLWidget() {}

void mGLWidget::initializeGL() {
    // Before this function, the opengl context has already been prepared.
    this->VAO = new QOpenGLVertexArrayObject(this);
    this->core_func = new QOpenGLFunctions_3_3_Core;

    this->core_func->initializeOpenGLFunctions();
    this->VAO->create();

    this->mesh_reader = new mMeshReader(this->VAO, this->core_func);
    this->mesh_reader->addMesh("/home/kaihang/Projects/QT/opengl_demo/meshes/sphere.ply");
    this->shader = new mShader("/home/kaihang/Projects/QT/opengl_demo/shaders/model_v.shader", "/home/kaihang/Projects/QT/opengl_demo/shaders/model_f.shader");

    glClearColor(0.4627450980392157f, 0.5882352941176471f, 0.8980392156862745f, 1.0f);
    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);
    this->timer_for_update->start(4);
}

void mGLWidget::resizeGL(int width, int height) {
    this->wnd_width = width;
    this->wnd_height = height;
    glViewport(0, 0, width, height);
}

void mGLWidget::mousePressEvent(QMouseEvent * event) {
    mArcBall::mouse_button_callback(event);
}

void mGLWidget::mouseMoveEvent(QMouseEvent *event) {
    mArcBall::mouse_move_callback(event);
}
void mGLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.4627450980392157f, 0.5882352941176471f, 0.8980392156862745f, 1.0f);
    this->draw();
    this->swapBuffers();
}

void mGLWidget::draw() {
    this->shader->use();
    glm::mat4 proj = glm::perspective(glm::radians(50.f), (float)this->wnd_width/this->wnd_height, 0.01f, 100.f);
    glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0.1f), glm::vec3(0, 1, 0));
    glm::mat4 model = mArcBall::getRotateMat(this->wnd_width, this->wnd_height, view);

    this->shader->setVal("MVP", proj * view * model);
    this->shader->setVal("modelMat", model);
    this->shader->setVal("normMat", glm::transpose(glm::inverse(model)));
    this->shader->setVal("lightPos", glm::vec3(10, 10, 10));
    this->shader->setVal("viewPos", glm::vec3(0, 0, 10));
    this->shader->setVal("fragColor", glm::vec3(1.f, 1.f, 1.f));

    this->mesh_reader->render(0);
}
