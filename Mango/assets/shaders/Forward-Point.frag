#version 450

#define POINT_LIGHT
#include "Forward-Lighting.glh"
#include "ParallaxMapping.glh"

layout(binding = 5) uniform samplerCubeShadow shadow_map;
uniform float s_far_plane;

uniform PointLight s_point_light;

vec3 sample_offset_directions[20] = vec3[]
(
    vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
    vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
    vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
    vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
    vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

float shadowCalculation(vec3 frag_pos)
{
    vec3 frag_to_light = (frag_pos - s_point_light.position);
    float compare = length(frag_to_light / s_far_plane);

    float bias = 0.00005;
    float shadow = 0.0;

    /* PCF */
    int samples = 20;
    float view_distance = length(g_cam_pos - frag_pos);
    float disk_radius = (1.0 + (view_distance / s_far_plane)) / 25.0;

    for(int i = 0; i < samples; ++i)
    {
        float pcf = texture(shadow_map, vec4(frag_to_light + sample_offset_directions[i] * disk_radius, compare - bias));
        shadow += pcf;
    }

    return shadow / float(samples);
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