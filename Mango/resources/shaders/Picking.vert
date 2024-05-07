#version 460

layout(location = 0) in vec3 aPos;

layout(location = 0) out flat int outID;

uniform mat4 g_mvp;
uniform int objectID;

void main()
{
    outID       = objectID;
    gl_Position = g_mvp * vec4(aPos, 1.0);
}