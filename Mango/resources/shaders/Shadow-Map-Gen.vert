#version 450

layout(location = 0) in vec3 a_position;

uniform mat4 g_model;
uniform mat4 s_light_matrix;

void main()
{
    gl_Position = s_light_matrix * g_model * vec4(a_position, 1.0f);
}