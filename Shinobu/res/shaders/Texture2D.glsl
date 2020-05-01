#type vertex
#version 450 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTex;

out vec2 tex;

uniform mat4 aVP;
uniform mat4 aTransform;

void main()
{
    tex = aTex;
    gl_Position =  aVP * aTransform * vec4(aPos, 0.f, 1.0f);
}

#type fragment
#version 450 core

uniform sampler2D sampl;
uniform vec4 aTint;

in vec2 tex;

out vec4 color;

void main()
{
	color = texture(sampl, tex) * aTint;
}