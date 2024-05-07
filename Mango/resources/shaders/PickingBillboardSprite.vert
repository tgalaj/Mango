#version 460 core

layout(location = 0) out flat int outID;

uniform mat4 g_view;
uniform vec3 position; // TODO(tgalaj): should be SSBO with the lights' data
uniform int objectID;

void main()
{
    outID       = objectID;
    gl_Position = g_view * vec4(position, 1.0);
}