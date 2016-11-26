#version 410 core

uniform vec3 debugDrawColor;
out vec4 color;

void main(void)
{
    color = vec4(debugDrawColor.rgb, 1);
}
