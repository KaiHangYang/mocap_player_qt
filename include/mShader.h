#ifndef M_SHADER 
#define M_SHADER
#include <glm/glm.hpp>
#include <string>

#include <QtOpenGL>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>

class mShader {
public:

    mShader() {}
    mShader(const QString &vs_path, const QString &fs_path);
    mShader(const QString &vs_path, const QString &fs_path, const QString &gs_path);

    ~mShader();
    void use();
    void setVal(const char * val_name, glm::vec3 val);
    void setVal(const char * val_name, glm::mat4 val);
    void setVal(const char * val_name, float val);
    void setVal(const char * val_name, unsigned int val);
    void setVal(const char * val_name, int val);

private:
    QOpenGLShaderProgram * program;
};

#endif
