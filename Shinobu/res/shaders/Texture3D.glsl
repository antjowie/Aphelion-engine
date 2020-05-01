#type vertex
#version 450 core

layout (location = 0) in vec3 aPos;

uniform mat4 aVP;
uniform mat4 aTransform;

void main()
{
    gl_Position =  aVP * aTransform * vec4(aPos, 1.0f);
}

#type fragment
#version 450 core

out vec4 color;

void main()
{
    color = vec4(1);
}