#version 410 core

#define MAX_LIGHTS 1

#define SOURCE_COLOR 0
#define SOURCE_TEXTURE 1

uniform struct Light {
    vec4 position;
    /*float diffuse_intensity;
    float ambient_intensity;
    float specular_intensity;
    float attenuation_coefficient;
    float cone_angle;
    vec3 cone_direction;*/
} lights[MAX_LIGHTS];

uniform struct Material {
    int diffuseType;
    vec3 diffuseColor;
    sampler2D diffuseTexture;

    bool useNormalMap;
    sampler2D normalMapTexture;

    vec3 ambientColor;

    int specularType;
    vec3 specularColor;
    sampler2D specularTexture;

    //vec3 emissive;

    float shininess;
    float transparency;
} material;

uniform bool allowTextureMaps = true;

in vec2 pass_TexCoords;
in vec3 pass_LightDirection_tangentspace;
in vec3 pass_EyeDirection_tangentspace;
in vec3 pass_SurfacePosition_worldspace;

in vec3 pass_Normal_cameraspace;
in vec3 pass_EyeDirection_cameraspace;
in vec3 pass_LightDirection_cameraspace;

in vec4 pass_Color;
out vec4 color;

void main(void)
{
    vec3 diffuseColor;
    if (material.diffuseType == SOURCE_COLOR) {
        diffuseColor = material.diffuseColor;
    } else {
        diffuseColor = texture2D(material.diffuseTexture, pass_TexCoords).rgb;
    }

    vec3 n, l, E;
    if (material.useNormalMap && allowTextureMaps) {
        n = normalize(texture2D(material.normalMapTexture, pass_TexCoords).rgb * 2.0 - 1.0);
        l = normalize(pass_LightDirection_tangentspace);
        E = normalize(pass_EyeDirection_tangentspace);
    } else {
        n = normalize(pass_Normal_cameraspace);
        l = normalize(pass_LightDirection_cameraspace);
        E = normalize(pass_EyeDirection_cameraspace);
    }

    vec3 specularColor;
    if (material.specularType == SOURCE_COLOR || !allowTextureMaps) {
        specularColor = material.specularColor;
    } else {
        specularColor = texture2D(material.specularTexture, pass_TexCoords).rgb * 0.4f;
    }

    float diffuse = clamp(dot(n, l), 0, 1);

    vec3 r = reflect(-l, n);
    float specular = clamp(dot(E, r), 0, 1);

    float lightPower = 50;
    float lightDistance = length(lights[0].position.xyz - pass_SurfacePosition_worldspace);

    //specular = clamp(specular, 0.000001, 0.00001);
    vec3 diffuseComponent = diffuseColor * diffuse;
    vec3 specularComponent = specularColor * pow(specular, material.shininess) * lightPower / (lightDistance*lightDistance);
    if (material.shininess == 0)
      specularComponent = vec3(0.0, 0.0, 0.0);

    color = vec4(diffuseComponent + specularComponent, 1.0f);

    //color = vec4(texture2D(texture, pass_TexCoords).rgb, 1.0) * pass_Color;
    //color = vec4(1, 1, 1, 1);
    //color = pass_Color;
}
