#version 410 core

in vec4 pass_Color;
out vec4 color;

void main(void)
{
    color = pass_Color;
}