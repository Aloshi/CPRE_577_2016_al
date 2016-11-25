#version 410 core

#define MAX_LIGHTS 1

uniform struct Light {
    vec4 position;
    float diffuse_intensity;
    float ambient_intensity;
    float specular_intensity;
    float attenuation_coefficient;
    float cone_angle;
    vec3 cone_direction;
} lights[MAX_LIGHTS];

uniform sampler2D texture;
uniform sampler2D normalTexture;
uniform sampler2D specularTexture;
//uniform float opacity;

uniform int useTexNormal = 1;
uniform int useSpec = 1;

in vec2 pass_TexCoords;
in vec3 pass_LightDirection_tangentspace;
in vec3 pass_EyeDirection_tangentspace;
in vec3 pass_SurfacePosition_worldspace;

in vec4 pass_Color;
out vec4 color;

void main(void)
{
    vec3 texNormal_tangentspace = normalize(texture2D(normalTexture, pass_TexCoords).rgb * 2.0 - 1.0);
    vec3 texNormal_tangentspace2 = vec3(0, 0, 1);
    texNormal_tangentspace = texNormal_tangentspace * useTexNormal + texNormal_tangentspace2 * (1 - useTexNormal);

    vec3 texColor = texture2D(texture, pass_TexCoords).rgb;
    vec3 specColor = texture2D(specularTexture, pass_TexCoords).rgb * 0.4f;

    vec3 n = normalize(texNormal_tangentspace);
    vec3 l = normalize(pass_LightDirection_tangentspace);
    float diffuse = clamp(dot(n, l), 0, 1);

    vec3 E = normalize(pass_EyeDirection_tangentspace);
    vec3 r = reflect(-l, n);
    float specular = clamp(dot(E, r), 0, 1);
    // vec3(1, 1, 1) * pow(specular, 120)

    float lightPower = 50;
    float distance = length(lights[0].position.xyz - pass_SurfacePosition_worldspace);

    color = vec4((texColor.rgb) * diffuse + specColor * pow(specular, 6) * useSpec * lightPower / (distance*distance), 1.0f);

    //color = vec4(texture2D(texture, pass_TexCoords).rgb, 1.0) * pass_Color;
    //color = vec4(1, 1, 1, 1);
    //color = pass_Color;
}
