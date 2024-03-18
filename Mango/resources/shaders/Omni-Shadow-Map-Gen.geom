#version 450

layout(triangles, invocations = 6) in;
layout(triangle_strip, max_vertices = 18) out;

uniform mat4 s_light_matrices[6];

out vec4 world_pos;

void main()
{
    for(int i = 0; i < 3; ++i)
    {
        world_pos   = gl_in[i].gl_Position;
        gl_Position = s_light_matrices[gl_InvocationID] * world_pos;
        gl_Layer    = gl_InvocationID;
        EmitVertex();
    }
    EndPrimitive();
}