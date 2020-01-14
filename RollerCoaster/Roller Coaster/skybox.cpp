#include <skybox.h>
#include <QtCore\qfileinfo.h>

SkyBox::SkyBox()
{
    d = new SkyBoxData;
}

SkyBox::~SkyBox()
{
    delete d;
}

void DimensionTransformation(GLfloat source[], GLfloat target[][4])
{
    //for uniform value, transfer 1 dimension to 2 dimension
    int i = 0;
    for (int j = 0; j < 4; j++)
        for (int k = 0; k < 4; k++)
        {
            target[j][k] = source[i];
            i++;
        }
}

void SkyBox::render(GLfloat* viewMatrix, GLfloat* projectionMatrix)
{
    this->init();

    d->shader->bind();
    d->vertexBuffer->bind();
    d->indexBuffer->bind();

    d->shader->enableAttributeArray("qt_Vertex");
    d->shader->setAttributeBuffer("qt_Vertex", GL_FLOAT, 0, 3, 0);

    d->shader->enableAttributeArray("qt_MultiTexCoord0");
    const int texCoordsOffset = 24 * sizeof(QVector3D);
    d->shader->setAttributeBuffer("qt_MultiTexCoord0", GL_FLOAT, texCoordsOffset, 2, 0);

    d->matrix.rotate(-1, 0, 1, 0);

    GLfloat P[4][4];
    GLfloat MV[4][4];
    DimensionTransformation(projectionMatrix, P);
    DimensionTransformation(viewMatrix, MV);
    //pass projection matrix to shader
    d->shader->setUniformValue("ProjectionMatrix", P);
    //pass modelview matrix to shader
    d->shader->setUniformValue("ModelViewMatrix", MV);

    for (int i = 0; i < 6; i++)
    {
        d->textures[i]->bind(i + 1);
        d->shader->setUniformValue("qt_Texture0", (i + 1));

        const uint triangleOffset = i * 6 * sizeof(uint);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)triangleOffset);

        d->textures[i]->release(i + 1);
    }

    d->indexBuffer->release();
    d->vertexBuffer->release();
    d->shader->release();
}

void SkyBox::init()
{
    if (d->shader)
        return;

    // Load all texture images
    const QImage posx = QImage("../../Textures/space_skybox_up.jpg").mirrored();
    const QImage posy = QImage("../../Textures/space_skybox_down.jpg").mirrored();
    const QImage posz = QImage("../../Textures/space_skybox_up.jpg").mirrored();
    const QImage negx = QImage("../../Textures/space_skybox_up.jpg").mirrored();
    const QImage negy = QImage("../../Textures/space_skybox_up.jpg").mirrored();
    const QImage negz = QImage("../../Textures/space_skybox_up.jpg").mirrored();

    // Load images as independent texture objects
    d->textures[0] = new QOpenGLTexture(posx);
    d->textures[1] = new QOpenGLTexture(posy);
    d->textures[2] = new QOpenGLTexture(posz);
    d->textures[3] = new QOpenGLTexture(negx);
    d->textures[4] = new QOpenGLTexture(negy);
    d->textures[5] = new QOpenGLTexture(negz);
    for (int i = 0; i < 6; i++)
    {
        d->textures[i]->setWrapMode(QOpenGLTexture::ClampToEdge);
        d->textures[i]->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
        d->textures[i]->setMagnificationFilter(QOpenGLTexture::Linear);
    }

    // Construct a template square of size 2x2
    const QVector3D p1(-1, 1, 0); // top-left
    const QVector3D p2(-1, -1, 0); // bottom-left
    const QVector3D p3(1, -1, 0); // bottom-right
    const QVector3D p4(1, 1, 0); // top-right

    // Array for storing geometry of the cube
    QVector<QVector3D> geometry;
    geometry.reserve(24);

    // Transform p1 ... p4 for posx
    QMatrix4x4 mat;
    mat.translate(1, 0, 0);
    mat.rotate(-90, 0, 1, 0);
    geometry << mat.map(p1) << mat.map(p2)
        << mat.map(p3) << mat.map(p4);

    // Transform p1 ... p4 for posy
    mat.setToIdentity();
    mat.translate(0, 1, 0);
    mat.rotate(90, 1, 0, 0);
    geometry << mat.map(p1) << mat.map(p2)
        << mat.map(p3) << mat.map(p4);

    // Transform p2 ... p4 for posz
    mat.setToIdentity();
    mat.translate(0, 0, -1);
    geometry << mat.map(p1) << mat.map(p2)
        << mat.map(p3) << mat.map(p4);

    // Transform p2 ... p4 for negx
    mat.setToIdentity();
    mat.translate(-1, 0, 0);
    mat.rotate(90, 0, 1, 0);
    geometry << mat.map(p1) << mat.map(p2)
        << mat.map(p3) << mat.map(p4);

    // Transform p2 ... p4 for negy
    mat.setToIdentity();
    mat.translate(0, -1, 0);
    mat.rotate(-90, 1, 0, 0);
    geometry << mat.map(p1) << mat.map(p2)
        << mat.map(p3) << mat.map(p4);

    // Transform p2 ... p4 for negz
    mat.setToIdentity();
    mat.translate(0, 0, 1);
    mat.rotate(180, 0, 1, 0);
    geometry << mat.map(p1) << mat.map(p2)
        << mat.map(p3) << mat.map(p4);

    // Texture coordinates
    QVector<QVector2D> texCoords;
    texCoords.reserve(24);
    for (int i = 0; i < 6; i++)
    {
        texCoords << QVector2D(0, 1)
            << QVector2D(0, 0)
            << QVector2D(1, 0)
            << QVector2D(1, 1);
    }

    // Triangles
    QVector<uint> triangles;
    triangles.reserve(36);
    for (int i = 0; i < 6; i++)
    {
        const int base = i * 4;
        triangles << base << base + 1 << base + 2;
        triangles << base << base + 2 << base + 3;
    }

    // Store the arrays in buffers
    d->vertexBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    d->vertexBuffer->create();
    d->vertexBuffer->bind();
    d->vertexBuffer->allocate(geometry.size() * sizeof(QVector3D) +
        texCoords.size() * sizeof(QVector2D));
    d->vertexBuffer->write(0, (const void*)geometry.constData(),
        geometry.size() * sizeof(QVector3D));
    d->vertexBuffer->write(geometry.size() * sizeof(QVector3D),
        (const void*)texCoords.constData(),
        texCoords.size() * sizeof(QVector2D));
    d->vertexBuffer->release();

    d->indexBuffer = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    d->indexBuffer->create();
    d->indexBuffer->bind();
    d->indexBuffer->allocate((const void*)triangles.constData(),
        triangles.size() * sizeof(uint));
    d->indexBuffer->release();

    // Create Shader
    d->shader = new QOpenGLShaderProgram();
    QFileInfo  vertexShaderFile("../../Shader/skybox_vertex.glsl");
    if (vertexShaderFile.exists())
    {
        d->vertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
        if (d->vertexShader->compileSourceFile("../../Shader/skybox_vertex.glsl"))
            d->shader->addShader(d->vertexShader);
        else
            qWarning() << "Vertex Shader Error " << d->vertexShader->log();
    }
    else
        qDebug() << vertexShaderFile.filePath() << " can't be found";

    QFileInfo  fragmentShaderFile("../../Shader/skybox_fragment.glsl.glsl");
    if (fragmentShaderFile.exists())
    {
        d->fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
        if (d->fragmentShader->compileSourceFile("../../Shader/skybox_fragment.glsl"))
            d->shader->addShader(d->fragmentShader);
        else
            qWarning() << "fragment Shader Error " << d->fragmentShader->log();
    }
    else
        qDebug() << fragmentShaderFile.filePath() << " can't be found";
    d->shader->link();
}