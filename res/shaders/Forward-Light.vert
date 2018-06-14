#version 450

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec3 a_texcoord;
layout(location = 3) in vec3 a_tangent;

out vec2 texcoord;
out vec3 world_pos;
out vec4 frag_pos_light_space;

out mat3 tbn;

uniform mat4 g_mvp;
uniform mat4 g_model;
uniform mat3 g_normal_matrix;
uniform mat4 s_light_matrix;

void main()
{
    world_pos            = (g_model * vec4(a_position, 1.0f)).xyz;
    texcoord             = a_texcoord.xy;
    frag_pos_light_space = s_light_matrix * vec4(world_pos, 1.0f);

    gl_Position = g_mvp * vec4(a_position, 1.0f);

    vec3 normal  = normalize(g_normal_matrix * a_normal);
    vec3 tangent = normalize(g_normal_matrix * a_tangent);

    /* Gram-Schmidt process */
    tangent = normalize(tangent - dot(tangent, normal) * normal);

    vec3 bitangent = cross(tangent, normal);
    tbn = mat3(tangent, bitangent, normal);
}