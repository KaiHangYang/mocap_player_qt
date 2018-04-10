#ifndef MGLWIDGET_H
#define MGLWIDGET_H

#include "mSceneUtils.h"
#include "mMoCapReader.h"

#include <QGLWidget>
#include <QtOpenGL>
#include <QTimer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions_3_3_Core>

class mGLWidget : public QGLWidget {
    Q_OBJECT
public:
    explicit mGLWidget(QGLFormat &gl_format, QWidget * parent=0, int wnd_width=960, int wnd_height=720);
    ~mGLWidget();

public slots:
    void changePoseFile(QString & file_name);
protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
private:
    void draw();

    glm::mat4 cam_in_mat;
    glm::mat4 cam_ex_mat;
    bool is_ar;

    int wnd_width;
    int wnd_height;

    mSceneUtils * scene;
    mMoCapData * mocap_data;
    mMoCapReader mocap_reader;
    QOpenGLVertexArrayObject * VAO;
    QOpenGLFunctions_3_3_Core * core_func;
    QTimer * timer_for_update;

};

#endif // MGLWIDGET_H
