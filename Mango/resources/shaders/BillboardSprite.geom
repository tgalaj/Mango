#version 460 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform float half_quad_width_vs; // in view-space
uniform mat4 g_projection;

out vec2 texcoords;

void main()
{
    gl_Position = g_projection * (vec4(-half_quad_width_vs, -half_quad_width_vs, 0.0, 0.0) + gl_in[0].gl_Position);
    texcoords = vec2(0.0, 0.0);
    EmitVertex();

    gl_Position = g_projection * (vec4(half_quad_width_vs, -half_quad_width_vs, 0.0, 0.0) + gl_in[0].gl_Position);
    texcoords = vec2(1.0, 0.0);
    EmitVertex();

    gl_Position = g_projection * (vec4(-half_quad_width_vs, half_quad_width_vs, 0.0, 0.0) + gl_in[0].gl_Position);
    texcoords = vec2(0.0, 1.0);
    EmitVertex();

    gl_Position = g_projection * (vec4(half_quad_width_vs, half_quad_width_vs, 0.0, 0.0) + gl_in[0].gl_Position);
    texcoords = vec2(1.0, 1.0);
    EmitVertex();

    EndPrimitive();
}