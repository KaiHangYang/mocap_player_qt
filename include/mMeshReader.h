#ifndef M_MESH_READER
#define M_MESH_READER

#include <iostream>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <QtOpenGL>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions_3_3_Core>

class mMeshEntry {
public:
    enum BUFFERS {
        VERTEX_BUFFER, TEXCOORD_BUFFER, NORMAL_BUFFER,INDEX_BUFFER
    };
    QOpenGLVertexArrayObject * VAO;
    QOpenGLBuffer * VBO[4];

    unsigned int elm_num;

    mMeshEntry(aiMesh * mesh, QOpenGLVertexArrayObject * VAO, QOpenGLFunctions_3_3_Core * core_func);
    ~mMeshEntry();

    void render();
    QOpenGLFunctions_3_3_Core * core_func;
};

class mMeshReader {
public:
    int mesh_size;
    std::vector<std::vector<mMeshEntry *>> mesh_entries;
    QOpenGLVertexArrayObject * VAO;

    mMeshReader(QOpenGLVertexArrayObject * VAO, QOpenGLFunctions_3_3_Core * core_func);
    ~mMeshReader();

    void addMesh(std::string mesh_file);
    void render(int mesh_num = 0);
private:
    QOpenGLFunctions_3_3_Core * core_func;
};


#endif
