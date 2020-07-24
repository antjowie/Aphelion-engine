#type vertex
#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;
layout (location = 3) in float aIndex;

uniform mat4 aVP;
uniform mat4 aTransform;

out vec2 tex;
out flat float texIndex;

void main()
{
    gl_Position =  aVP * aTransform * vec4(aPos, 1.0f);
    tex = aTex;
    texIndex = aIndex;
}

#type fragment
#version 450 core

in vec2 tex;
in flat float texIndex;
out vec4 color;

uniform sampler2DArray textureSampler;

void main()
{
    color = texture(textureSampler, vec3(tex, texIndex));
}