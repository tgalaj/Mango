#version 460

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexcoord;

layout(location = 0) out flat int outID;
layout(location = 1) out vec2 texcoord;

uniform mat4 g_mvp;
uniform int objectID;

void main()
{
    outID       = objectID;
    texcoord    = aTexcoord;
    gl_Position = g_mvp * vec4(aPos, 1.0);
}