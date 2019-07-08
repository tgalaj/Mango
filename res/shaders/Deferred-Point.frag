#version 450
#define POINT_LIGHT
#include "Deferred-Lighting.glh"

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

vec2 calcTexCoord()
{
    return gl_FragCoord.xy / vec2(textureSize(gbuffer_positions, 0));
}

void main()
{
    texcoord = calcTexCoord();

    vec4 albedo    = vec4(texture(gbuffer_albedo_spec, texcoord).rgb, 1.0f);
    vec4 ambient   = vec4(s_scene_ambient, 1.0f);
    vec3 world_pos = texture(gbuffer_positions,   texcoord).xyz;
    vec3 normal    = texture(gbuffer_normals,     texcoord).xyz;

    float shadow = shadowCalculation(world_pos);

    light_info = (ambient + shadow * calcPointLight(s_point_light, normal, world_pos)) * albedo;
}