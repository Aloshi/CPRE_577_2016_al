#version 410 core

//uniform sampler2D texture;
//uniform float opacity;

//in vec2 pass_TexCoords;

in vec4 pass_Color;
out vec4 color;

void main(void)
{
    //color = vec4(texture2D(texture, pass_TexCoords).rgb, opacity);
    //color = vec4(1, 1, 1, 1);
    color = pass_Color;
}
