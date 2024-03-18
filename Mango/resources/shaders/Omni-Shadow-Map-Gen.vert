#version 450

layout(location = 0) in vec3 a_position;

uniform mat4 g_model;

void main()
{
    gl_Position = g_model * vec4(a_position, 1.0f);
}