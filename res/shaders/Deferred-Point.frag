#version 450

vec2 texcoord;
#include "Deferred-Lighting.glh"

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

vec2 calcTexCoord()
{
    return gl_FragCoord.xy / vec2(textureSize(gbuffer_positions, 0));
}

void main()
{
    texcoord = calcTexCoord();

    vec4 albedo    = vec4(texture(gbuffer_albedo_spec, texcoord).rgb, 1.0f);
    vec4 ambient   = vec4(s_scene_ambient * texture(ambient_occlusion_texture, texcoord).r, 1.0f);
    vec3 world_pos = texture(gbuffer_positions,   texcoord).xyz;
    vec3 normal    = texture(gbuffer_normals,     texcoord).xyz;

    float shadow = shadowCalculation(world_pos);

    light_info = (ambient + shadow * calcPointLight(s_point_light, normal, world_pos)) * albedo;
}