#version 430 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 color;

uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;

out vec4 vColor;

void main(void)
{
    gl_Position = ProjectionMatrix * ModelViewMatrix * vec4(vertex, 1.0);
    vColor = vec4(color,1);
}
