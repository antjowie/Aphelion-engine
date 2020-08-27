#type vertex
#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;

uniform mat4 aVP;
uniform mat4 aTransform;

out vec2 tex;

void main()
{
    gl_Position =  aVP * aTransform * vec4(aPos, 1.0f);
    tex = aTex;
}

#type fragment
#version 450 core

in vec2 tex;
out vec4 color;

uniform sampler2D textureSampler;

void main()
{
    color = texture(textureSampler,tex);
}