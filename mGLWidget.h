#ifndef MGLWIDGET_H
#define MGLWIDGET_H


#include "mMeshReader.h"
#include "mShader.h"

#include <QGLWidget>
#include <QtOpenGL>
#include <QTimer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions_3_3_Core>

class mGLWidget : public QGLWidget {
    Q_OBJECT
public:
    explicit mGLWidget(QWidget * parent=0, int wnd_width=960, int wnd_height=720);
    ~mGLWidget();
protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
private:
    void draw();

    QOpenGLVertexArrayObject * VAO;
    int wnd_width;
    int wnd_height;

    mShader * shader;
    mMeshReader * mesh_reader;
    QOpenGLFunctions_3_3_Core * core_func;
    QTimer * timer_for_update;
};

#endif // MGLWIDGET_H
