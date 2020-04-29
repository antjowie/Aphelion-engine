#type vertex
#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec2 tex;

uniform mat4 aVP;
uniform mat4 aModel;

void main()
{
    tex = aTex;
    //gl_Position = vec4(aPos * aVP * aModel,1.0f);
    gl_Position = vec4(aPos,1.0f);
}

#type fragment
#version 450 core

uniform sampler2D sampl;

in vec2 tex;
out vec4 color;

void main()
{
	color = texture(sampl, tex);
}