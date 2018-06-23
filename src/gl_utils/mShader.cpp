#include "mShader.h"

#include <vector>
#include <QDebug>
#include <QFileInfo>
#include <QMatrix>

mShader::~mShader() {
    this->program->release();
}
mShader::mShader(const QString &vs_path, const QString &fs_path) {
    QFileInfo v_file(vs_path);
    QFileInfo f_file(fs_path);

    if (!v_file.isFile() || !f_file.isFile()) {
        qDebug() << "Shader file is not exist!";
    }

    QOpenGLShader vertex_shader(QOpenGLShader::Vertex);
    QOpenGLShader frag_shader(QOpenGLShader::Fragment);

    vertex_shader.compileSourceFile(vs_path);
    frag_shader.compileSourceFile(fs_path);

    if (!vertex_shader.isCompiled()) {
        qDebug() << vertex_shader.log();
    }

    if (!frag_shader.isCompiled()) {
        qDebug() << frag_shader.log();
    }

    // Link the program
    this->program = new QOpenGLShaderProgram;
    this->program->addShader(&vertex_shader);
    this->program->addShader(&frag_shader);

    this->program->link();
    if (!this->program->isLinked()) {
        qDebug() << this->program->log();
    }
}

mShader::mShader(const QString &vs_path, const QString &fs_path, const QString &gs_path) {
    QFileInfo v_file(vs_path);
    QFileInfo f_file(fs_path);
    QFileInfo g_file(gs_path);

    if (!v_file.isFile() || !f_file.isFile() || !g_file.isFile()) {
        qDebug() << "Shader file is not exist!";
    }

    QOpenGLShader vertex_shader(QOpenGLShader::Vertex);
    QOpenGLShader frag_shader(QOpenGLShader::Fragment);
    QOpenGLShader geo_shader(QOpenGLShader::Geometry);

    vertex_shader.compileSourceFile(vs_path);
    frag_shader.compileSourceFile(fs_path);
    geo_shader.compileSourceFile(gs_path);

    if (!vertex_shader.isCompiled()) {
        qDebug() << vertex_shader.log();
    }

    if (!frag_shader.isCompiled()) {
        qDebug() << frag_shader.log();
    }

    if (!geo_shader.isCompiled()) {
        qDebug() << geo_shader.log();
    }

    // Link the program
    this->program = new QOpenGLShaderProgram;
    this->program->addShader(&vertex_shader);
    this->program->addShader(&frag_shader);
    this->program->addShader(&geo_shader);

    this->program->link();
    if (!this->program->isLinked()) {
        qDebug() << this->program->log();
    }
}
void mShader::use() {
    this->program->bind();
}
void mShader::setVal(const char * val_name, glm::mat4 val) {
    val = glm::transpose(val);
    QMatrix4x4 q_val(&val[0][0]);
    this->program->setUniformValue(val_name, q_val);
}
void mShader::setVal(const char * val_name, unsigned int val) {
    this->program->setUniformValue(val_name, val);
}
void mShader::setVal(const char * val_name, float val) {
    this->program->setUniformValue(val_name, val);
}
void mShader::setVal(const char * val_name, int val) {
    this->program->setUniformValue(val_name, val);
}
void mShader::setVal(const char * val_name, glm::vec3 val) {
    QVector3D q_val(val[0], val[1], val[2]);
    this->program->setUniformValue(val_name, q_val);
}
void mShader::setVal(const char * val_name, bool val) {
    this->program->setUniformValue(val_name, val);
}
