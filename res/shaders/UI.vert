#version 450

layout(location = 0) in vec4 a_vertex; // <vec2 pos, vec2 texcoord>

out vec2 texcoord;

uniform mat4 projection;

void main()
{
    texcoord = a_vertex.zw;
    gl_Position = projection * vec4(a_vertex.xy, 0.0f, 1.0f);
}