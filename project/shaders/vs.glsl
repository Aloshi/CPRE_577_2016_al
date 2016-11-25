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

uniform struct Material {
    vec3 diffuse;
    vec3 ambient;
    vec3 specular;
    //vec3 emissive;
    float shininess;
    float transparency;
} materials[1];

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

in vec3 in_Position;
in vec3 in_Normal;
in vec2 in_TexCoords;
in vec3 in_Tangent;
in vec3 in_Bitangent;
out vec2 pass_TexCoords;
out vec3 pass_LightDirection_tangentspace;
out vec3 pass_EyeDirection_tangentspace;
out vec3 pass_SurfacePosition_worldspace;

//out vec4 pass_Color;

/*vec4 useLight(Light light, vec4 surfacePosition, vec4 normal_transformed, vec3 normal, mat4 inv_view, Material material)
{
    // Calculate the vector from surface to the current light
    vec4 surface_to_light = normalize(light.position - surfacePosition);
    if (light.position.w == 0.0) {
        surface_to_light = normalize(light.position);
    }

    // Diffuse color
    float diffuse_coefficient = max(dot(vec4(normal, 0), surface_to_light), 0.0);
    vec3 out_diffuse_color = material.diffuse * diffuse_coefficient * light.diffuse_intensity;

    // Ambient color
    vec3 out_ambient_color = material.ambient * light.ambient_intensity;

    // Specular color
    vec3 incidenceVector = -surface_to_light.xyz;
    vec3 reflectionVector = reflect(incidenceVector, normal.xyz);
    vec3 cameraPosition = vec3(inv_view[3][0], inv_view[3][1], inv_view[3][2]);
    vec3 surfaceToCamera = normalize(cameraPosition - surfacePosition.xyz);
    float cosAngle = max(dot(surfaceToCamera, reflectionVector), 0.0);
    float specular_coefficient = pow(cosAngle, material.shininess);
    vec3 out_specular_color = material.specular * specular_coefficient * light.specular_intensity;

    //attenuation
    float distanceToLight = length(light.position.xyz - surfacePosition.xyz);
    float attenuation = 1.0 / (1.0 + light.attenuation_coefficient * pow(distanceToLight, 2));

    if(light.position.w == 1.0)
    {
        //////////////////////////////////////////////////////////////////////////////////////////////
        // Spotlight
        // 1. Normalize the cone direction
        vec3 cone_direction_norm = normalize(light.cone_direction);

        // 2. Calculate the ray direction. We already calculated the surface to light direction.
        //    All what we need to do is to inverse this value
        vec3 ray_direction = -surface_to_light.xyz;

        // 3. Calculate the angle between light and surface using the dot product again.
        //    To simplify our understanding, we use the degrees
        float light_to_surface_angle = degrees(acos(dot(ray_direction, cone_direction_norm))) ;

        // 4. Last, we compare the angle with the current direction and
        //    reduce the attenuation to 0.0 if the light is outside the angle.
        if (light_to_surface_angle > light.cone_angle) {
            attenuation = 0.0;
        }
    } else if (light.position.w == 0.0) {
        //////////////////////////////////////////////////////////////////////////////////////////////
        // Directional light

        // 1. the values that we store as light position is our light direction.
        vec3 light_direction = normalize(light.position.xyz);

        // 2. We check the angle of our light to make sure that only parts towards our light get illuminated
        float light_to_surface_angle = dot(light_direction, normal_transformed.xyz);

        // 3. Check the angle, if the angle is smaller than 0.0, the surface is not directed towards the light.
        //if(light_to_surface_angle > 0.0)attenuation = 1.0;
        //else attenuation = 0.0;
        attenuation = 1.0;
    }

    // Calculate the linear color
    vec3 linearColor = out_ambient_color + attenuation * (out_diffuse_color + out_specular_color);

    // adds transparency to the object
    return vec4(linearColor, material.transparency);
}*/

void main(void)
{
    mat4 modelViewMat = viewMatrix * modelMatrix;

    vec3 position_cameraspace = (modelViewMat * vec4(in_Position, 1)).xyz;
    vec3 normal_cameraspace = (modelViewMat * vec4(in_Normal, 0)).xyz;
    vec3 tangent_cameraspace = (modelViewMat * vec4(in_Tangent, 0)).xyz;
    vec3 bitangent_cameraspace = (modelViewMat * vec4(in_Bitangent, 0)).xyz;

    vec3 lightPosition_cameraspace = (viewMatrix * vec4(lights[0].position.xyz, 1)).xyz;

    mat3 TBN = inverse(mat3(
      tangent_cameraspace,
      bitangent_cameraspace,
      normal_cameraspace
    ));

    vec3 eye_direction_cameraspace = vec3(0, 0, 0) - position_cameraspace;  // vertex -> camera
    vec3 light_direction_cameraspace = lightPosition_cameraspace + eye_direction_cameraspace;  // vertex -> light

    pass_LightDirection_tangentspace = TBN * light_direction_cameraspace;
    pass_EyeDirection_tangentspace = TBN * vec3(0, 1, 0); //eye_direction_cameraspace;

    pass_SurfacePosition_worldspace = (modelMatrix * vec4(in_Position, 1.0)).xyz;

/*    vec3 normal = normalize(in_Normal);
    vec4 transformedNormal =  normalize(transpose(inverse(modelMatrix)) * vec4(normal, 1.0));
    vec4 surfacePosition = modelMatrix * vec4(in_Position, 1.0);*/

    // Calculate the color
/*  
    mat4 inv_view = inverse(viewMatrix);
    vec4 linearColor = vec4(0.0, 0.0, 0.0, 0.0);
    linearColor += useLight(lights[0], surfacePosition, transformedNormal, normal, inv_view, materials[0]);


    vec4 gamma = vec4(1.0/2.2);
    vec4 finalColor = pow(linearColor, gamma);
    pass_Color = vec4(finalColor);*/

    pass_TexCoords = in_TexCoords;
    gl_Position = projectionMatrix * modelViewMat * vec4(in_Position, 1.0);
}
