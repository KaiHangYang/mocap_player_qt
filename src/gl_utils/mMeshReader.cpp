#include "mMeshReader.h"
#include <QDebug>

mMeshEntry::~mMeshEntry() {
    this->VAO->bind();
    if (this->VBO[VERTEX_BUFFER]->isCreated()) {
        this->VBO[VERTEX_BUFFER]->destroy();
    }

    if (this->VBO[TEXCOORD_BUFFER]->isCreated()) {
        this->VBO[TEXCOORD_BUFFER]->destroy();
    }

    if (this->VBO[NORMAL_BUFFER]->isCreated()) {
        this->VBO[NORMAL_BUFFER]->destroy();
    }

    if (this->VBO[INDEX_BUFFER]->isCreated()) {
        this->VBO[INDEX_BUFFER]->destroy();
    }

    this->VBO[VERTEX_BUFFER]->~QOpenGLBuffer();
    this->VBO[TEXCOORD_BUFFER]->~QOpenGLBuffer();
    this->VBO[NORMAL_BUFFER]->~QOpenGLBuffer();
    this->VBO[INDEX_BUFFER]->~QOpenGLBuffer();

    this->VAO->release();
}

mMeshEntry::mMeshEntry(aiMesh * mesh, QOpenGLVertexArrayObject * vao, QOpenGLFunctions_3_3_Core * core_func) {
    this->VAO = vao;
    this->core_func = core_func;
    this->VBO[VERTEX_BUFFER]= new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    this->VBO[INDEX_BUFFER] = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    this->VBO[NORMAL_BUFFER] = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    this->VBO[TEXCOORD_BUFFER] = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);

    this->VAO->bind();

    if (mesh->HasPositions()) {
        std::vector<GLfloat> vertexs;
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            vertexs.push_back(mesh->mVertices[i].x);
            vertexs.push_back(mesh->mVertices[i].y);
            vertexs.push_back(mesh->mVertices[i].z);
        }
        
        this->VBO[VERTEX_BUFFER]->create();
        this->VBO[VERTEX_BUFFER]->bind();
        this->VBO[VERTEX_BUFFER]->setUsagePattern(QOpenGLBuffer::StaticDraw);

        this->VBO[VERTEX_BUFFER]->allocate(&vertexs[0], vertexs.size() * sizeof(GLfloat));
    }

    if (mesh->HasFaces()) {
        std::vector<unsigned int> indices;
        for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; ++j) {
                indices.push_back(face.mIndices[j]);
            }
        }
        this->VBO[INDEX_BUFFER]->create();
        this->VBO[INDEX_BUFFER]->bind();
        this->VBO[INDEX_BUFFER]->setUsagePattern(QOpenGLBuffer::StaticDraw);

        this->VBO[INDEX_BUFFER]->allocate(&indices[0], indices.size() * sizeof(unsigned int));
        this->elm_num = indices.size();
    }

    if (mesh->HasNormals()) {
        std::vector<GLfloat> normals;
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            normals.push_back(mesh->mNormals[i].x);
            normals.push_back(mesh->mNormals[i].y);
            normals.push_back(mesh->mNormals[i].z);
        }
        this->VBO[NORMAL_BUFFER]->create();
        this->VBO[NORMAL_BUFFER]->bind();
        this->VBO[NORMAL_BUFFER]->setUsagePattern(QOpenGLBuffer::StaticDraw);

        this->VBO[NORMAL_BUFFER]->allocate(&normals[0], normals.size() * sizeof(GLfloat));
    }

    this->VAO->release();
}

void mMeshEntry::render() {
    this->VAO->bind();

    if (this->VBO[VERTEX_BUFFER]->isCreated()) {
        this->VBO[VERTEX_BUFFER]->bind();
        this->core_func->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)(0));
        this->core_func->glEnableVertexAttribArray(0);
    }
    if (this->VBO[NORMAL_BUFFER]->isCreated()) {
        this->VBO[NORMAL_BUFFER]->bind();
        this->core_func->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
        this->core_func->glEnableVertexAttribArray(2);
        
    }
    if (this->VBO[INDEX_BUFFER]->isCreated()) {
        this->VBO[INDEX_BUFFER]->bind();
        this->core_func->glVertexAttribPointer(1, 3, GL_UNSIGNED_INT, GL_FALSE, 0, (void *)0);
        this->core_func->glEnableVertexAttribArray(1);
    }
    this->core_func->glDrawElements(GL_TRIANGLES, this->elm_num, GL_UNSIGNED_INT, NULL);

    this->core_func->glDisableVertexAttribArray(0);
    this->core_func->glDisableVertexAttribArray(2);
    this->core_func->glDisableVertexAttribArray(1);

    this->VAO->release();
}

mMeshReader::mMeshReader(QOpenGLVertexArrayObject * vao, QOpenGLFunctions_3_3_Core * core_func) {
    this->core_func = core_func;
    this->VAO = vao;
    this->mesh_size = 0;
}
void mMeshReader::addMesh(std::string mesh_file) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(mesh_file, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        qDebug() << "(mMeshReader.cpp): " << importer.GetErrorString();
    }

    this->mesh_entries.push_back(std::vector<mMeshEntry *>());
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        this->mesh_entries[this->mesh_size].push_back(new mMeshEntry(scene->mMeshes[i], this->VAO, this->core_func));
    }
    ++this->mesh_size;
}
mMeshReader::~mMeshReader() {
    for (unsigned int i = 0; i < this->mesh_entries.size(); ++i) {
        for (unsigned int j = 0; j < this->mesh_entries[i].size(); ++j) {
            this->mesh_entries[i][j]->~mMeshEntry();
        }
        this->mesh_entries[i].clear();
    }
    this->mesh_entries.clear();
    this->VAO->release();
}
void mMeshReader::render(int mesh_num) {
    if (mesh_num >= this->mesh_size) {
        qDebug() << "(mMeshReader.cpp): mesh number is exceed the bound.(cur:"+
                    QString(mesh_num)+", max:"+QString(this->mesh_size-1)+")";
        exit(-1);
    }
    for (unsigned int i = 0; i < this->mesh_entries[mesh_num].size(); ++i) {
        this->mesh_entries[mesh_num][i]->render();
    }
}
