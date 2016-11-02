#version 410 core

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

in vec3 in_Position;
in vec2 in_TexCoords;

out vec2 pass_TexCoords;

void main(void)
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(in_Position, 1.0);
    pass_TexCoords = in_TexCoords;
}