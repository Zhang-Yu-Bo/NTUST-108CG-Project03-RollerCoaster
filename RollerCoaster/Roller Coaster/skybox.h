#pragma once
#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QtGui\qopenglfunctions.h>

struct SkyBoxData
{
    SkyBoxData() {
        for (int i = 0; i < 6; i++)
            this->textures[i] = 0;
        this->vertexBuffer = 0;
        this->indexBuffer = 0;
        this->shader = 0;
    }
    ~SkyBoxData() {
        delete this->shader;
        delete this->indexBuffer;
        delete this->vertexBuffer;
        for (int i = 5; i >= 0; i--)
            delete this->textures[i];
    }

    QOpenGLTexture* textures[6];
    QOpenGLBuffer* vertexBuffer;
    QOpenGLBuffer* indexBuffer;
    QOpenGLShaderProgram* shader;
    QMatrix4x4 matrix;

    QOpenGLShader* vertexShader;
    QOpenGLShader* fragmentShader;
};


class SkyBox {
public:
	SkyBox();
	~SkyBox();
	
	void init();
	void render(GLfloat* viewMatrix, GLfloat* projectionMatrix);

	SkyBoxData* d;
};