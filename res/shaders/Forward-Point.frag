#version 450

#define POINT_LIGHT
#include "Forward-Lighting.glh"
#include "ParallaxMapping.glh"

layout(binding = 5) uniform samplerCubeShadow shadow_map;
uniform float s_far_plane;

uniform PointLight s_point_light;

float shadowCalculation(vec3 frag_pos)
{
    vec3 frag_to_light = (frag_pos - s_point_light.position);
    float compare = length(frag_to_light / s_far_plane);

    float bias = 0.00005; 
    float shadow = texture(shadow_map, vec4(frag_to_light, compare - bias));

    return shadow;
}

void main()
{
    vec3 dir_to_eye = normalize(g_cam_pos - world_pos) * tbn;
    parallax_texcoord = parallaxMapping(dir_to_eye);

    vec4 diffuse_tex_color = texture(m_texture_diffuse, parallax_texcoord);

    vec3 normal = texture(m_texture_normal, parallax_texcoord).rgb;
    normal = tbn * (normal * 2.0f - 1.0f);

    float shadow = shadowCalculation(world_pos);

    frag_color = shadow * diffuse_tex_color * calcPointLight(s_point_light, normalize(normal), world_pos);
}