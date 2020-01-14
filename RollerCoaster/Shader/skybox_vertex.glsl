attribute vec4 qt_Vertex;
attribute vec4 qt_MultiTexCoord0;
uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;
varying vec4 qt_TexCoord0;

void main(void)
{
    gl_Position = ProjectionMatrix * ModelViewMatrix * qt_Vertex;
    qt_TexCoord0 = qt_MultiTexCoord0;
}
