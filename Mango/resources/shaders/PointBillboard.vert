#version 460 core

uniform vec3 position; // TODO(tgalaj): should be SSBO with the lights' data
uniform mat4 g_view;

void main()
{
    gl_Position = g_view * vec4(position, 1.0);
}