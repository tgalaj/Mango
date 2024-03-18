#version 460 core

layout(location = 0) in vec3 a_position;

uniform mat4 g_mvp;

void main()
{
    gl_Position = g_mvp * vec4(a_position, 1.0f);
}