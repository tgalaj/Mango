#version 450

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texcoord;

out vec2 texcoord;

uniform mat4 g_mvp;

void main()
{
    texcoord = a_texcoord;

    gl_Position = g_mvp * vec4(a_position, 1.0f);
}