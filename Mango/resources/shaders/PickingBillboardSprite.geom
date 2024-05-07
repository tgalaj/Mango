#version 460 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

layout(location = 0) in  flat int inID[];
layout(location = 0) out flat int outID;

uniform float half_quad_width_vs; // in view-space
uniform mat4 g_projection;

void main()
{
    outID = inID[0];

    gl_Position = g_projection * (vec4(-half_quad_width_vs, -half_quad_width_vs, 0.0, 0.0) + gl_in[0].gl_Position);
    EmitVertex();

    gl_Position = g_projection * (vec4(half_quad_width_vs, -half_quad_width_vs, 0.0, 0.0) + gl_in[0].gl_Position);
    EmitVertex();

    gl_Position = g_projection * (vec4(-half_quad_width_vs, half_quad_width_vs, 0.0, 0.0) + gl_in[0].gl_Position);
    EmitVertex();

    gl_Position = g_projection * (vec4(half_quad_width_vs, half_quad_width_vs, 0.0, 0.0) + gl_in[0].gl_Position);
    EmitVertex();

    EndPrimitive();
}