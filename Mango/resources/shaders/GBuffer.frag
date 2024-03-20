#version 460 core

in vec2 texcoord;
in vec3 world_pos;
in mat3 tbn;

uniform vec3 g_cam_pos;
uniform float alpha_cutoff;

layout(binding = 0) uniform sampler2D m_texture_diffuse;
layout(binding = 1) uniform sampler2D m_texture_specular;
layout(binding = 2) uniform sampler2D m_texture_normal;
layout(binding = 3) uniform sampler2D m_texture_emission;
layout(binding = 4) uniform sampler2D m_texture_depth;

vec2 parallax_texcoord;

layout (location = 0) out vec3 positions;
layout (location = 1) out vec3 normals;
layout (location = 2) out vec4 albedo_specular;

#include "ParallaxMapping.glh"

void main()
{
    vec3 dir_to_eye = normalize(g_cam_pos - world_pos) * tbn;
    parallax_texcoord = parallaxMapping(dir_to_eye);

    vec4 diffuse_tex_color = texture(m_texture_diffuse, parallax_texcoord);

    if(diffuse_tex_color.a < alpha_cutoff)
    {
        discard;
    }

    vec3 normal = texture(m_texture_normal, parallax_texcoord).rgb;
    normal = normalize(tbn * (normal * 2.0f - 1.0f));

    positions           = world_pos;
    normals             = normal;
    albedo_specular.rgb = diffuse_tex_color.rgb;
    albedo_specular.a   = texture(m_texture_specular, parallax_texcoord).r;
}