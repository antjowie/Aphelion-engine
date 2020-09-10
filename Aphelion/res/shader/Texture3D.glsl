#type vertex
#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;

uniform mat4 aVP;
uniform mat4 aTransform;

out vec2 tex;
out vec3 normal;

void main()
{
    gl_Position =  aVP * aTransform * vec4(aPos, 1.0f);
    tex = aTex;

    normal = vec3(aTransform * vec4(aNormal,0));
    normal = normalize(normal);
}

#type fragment
#version 450 core

in vec2 tex;
in vec3 normal;
out vec4 finalColor;

uniform vec3 aLightDir;
uniform float aAmbient;
uniform sampler2D textureSampler;

void main()
{
    // Lambertian
    // https://en.wikipedia.org/wiki/Lambertian_reflectance
    vec3 l = -aLightDir;
    vec3 n = normal;
    float intensity = 1;
    // vec3 lColor = vec3(1);
    float lamb = dot(l,n) * intensity;

    vec4 color = texture(textureSampler,tex);

    finalColor = aAmbient * color + lamb * color;
    finalColor.a = 1;
    // color = vec4(n,1);
}