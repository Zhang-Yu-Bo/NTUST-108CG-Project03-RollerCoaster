#include "Square.h"

Square::Square()
{
}
void Square::DimensionTransformation(GLfloat source[],GLfloat target[][4])
{
	//for uniform value, transfer 1 dimension to 2 dimension
	int i = 0;
	for(int j=0;j<4;j++)
		for(int k=0;k<4;k++)
		{
			target[j][k] = source[i];
			i++;
		}
}
void Square::Begin()
{
	//Bind the shader we want to draw with
	shaderProgram->bind();
	//Bind the VAO we want to draw
	vao.bind();
}
void Square::Paint(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix)
{
	GLfloat P[4][4];
	GLfloat MV[4][4];
	DimensionTransformation(ProjectionMatrix,P);
	DimensionTransformation(ModelViewMatrix,MV);
	//pass projection matrix to shader
	shaderProgram->setUniformValue("ProjectionMatrix",P);
	//pass modelview matrix to shader
	shaderProgram->setUniformValue("ModelViewMatrix",MV);

	// Bind the buffer so that it is the current active buffer
	vvbo.bind();
	// Enable Attribute 0
	shaderProgram->enableAttributeArray(0);
	// Set Attribute 0 to be position
	shaderProgram->setAttributeArray(0,GL_FLOAT,0,3,NULL);
	//unbind buffer
	vvbo.release();

	// Bind the buffer so that it is the current active buffer
	uvbo.bind();
	// Enable Attribute 1
	shaderProgram->enableAttributeArray(1);

	shaderProgram->setAttributeArray(1,GL_FLOAT,0,2,NULL);
	uvbo.release();

	//Draw triangles with 4 indices starting from the 0th index
	glDrawArrays(GL_TRIANGLE_FAN,0,vertices.size());
}
void Square::End()
{
	//Disable Attribute 0&1
	shaderProgram->disableAttributeArray(0);
	shaderProgram->disableAttributeArray(1);

	vao.release();
	shaderProgram->release();
}

void Square::Init()
{
	InitShader("./Shader/Square.vs","./Shader/Square.fs");
	InitVAO();
	InitVBO();
	//InitTexture();
}
void Square::InitVAO()
{
	 // Create Vertex Array Object
	vao.create();
	// Bind the VAO so it is the current active VAO
	vao.bind();
}
void Square::InitVBO()
{
	//Set each vertex's position
	vertices <<QVector3D(0.0f, 2.0f, -10.0f)  
			<<QVector3D(0.0f, 2.0f, 10.0f)
			<<QVector3D(-20.0f, 2.0f, 10.0f)
			<<QVector3D(-20.0f, 2.0f, -10.0f);
	// Create Buffer for position
	vvbo.create();
	// Bind the buffer so that it is the current active buffer
	vvbo.bind();
	// Since we will never change the data that we are about to pass the Buffer, we will say that the Usage Pattern is StaticDraw
	vvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	// Allocate and initialize the information
	vvbo.allocate(vertices.constData(),vertices.size() * sizeof(QVector3D));

	//Set each vertex's uv
	uvs<<QVector2D(0.0f,0.0f)
		<<QVector2D(0.0f,1.0f)
		<<QVector2D(1.0f,1.0f)
		<<QVector2D(1.0f,.0f);
	// Create Buffer for uv
	uvbo.create();
	// Bind the buffer so that it is the current active buffer
	uvbo.bind();
	// Since we will never change the data that we are about to pass the Buffer, we will say that the Usage Pattern is StaticDraw
	uvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	// Allocate and initialize the information
	uvbo.allocate(uvs.constData(),uvs.size() * sizeof(QVector2D));

}
void Square::InitShader(QString vertexShaderPath,QString fragmentShaderPath)
{
	// Create Shader
	shaderProgram = new QOpenGLShaderProgram();
	QFileInfo  vertexShaderFile(vertexShaderPath);
	if(vertexShaderFile.exists())
	{
		vertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
		if(vertexShader->compileSourceFile(vertexShaderPath))
			shaderProgram->addShader(vertexShader);
		else
			qWarning() << "Vertex Shader Error " << vertexShader->log();
	}
	else
		qDebug()<<vertexShaderFile.filePath()<<" can't be found";

	QFileInfo  fragmentShaderFile(fragmentShaderPath);
	if(fragmentShaderFile.exists())
	{
		fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
		if(fragmentShader->compileSourceFile(fragmentShaderPath))
			shaderProgram->addShader(fragmentShader);
		else
			qWarning() << "fragment Shader Error " << fragmentShader->log();
	}
	else
		qDebug()<<fragmentShaderFile.filePath()<<" can't be found";
	shaderProgram->link();
}