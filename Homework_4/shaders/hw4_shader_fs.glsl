#version 410 core

uniform sampler2D texture;
uniform float opacity;

in vec2 pass_TexCoords;

out vec4 color;

void main(void)
{
    color = vec4(texture2D(texture, pass_TexCoords).rgb, opacity);
}