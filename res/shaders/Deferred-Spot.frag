#version 450

vec2 texcoord;
#include "Deferred-Lighting.glh"

layout(binding = 5) uniform sampler2DShadow shadow_map;

uniform SpotLight s_spot_light;
uniform mat4 s_light_matrix;

float shadowCalculation(vec4 frag_pos_light_space, vec3 normal, vec3 world_pos)
{
    vec3 proj_coords = frag_pos_light_space.xyz / frag_pos_light_space.w;
    proj_coords      = proj_coords * 0.5f + 0.5f; /* Map from [-1, 1] to [0, 1] */

    if(proj_coords.z > 1.0f)
        return 0.0f;

    vec3 dir_to_frag = normalize(world_pos - s_spot_light.point.position);
    float bias = max(0.000005f * (1.0 - dot(normal, -dir_to_frag)), 0.0000005f); /* Removes shadow acne artifact */
    proj_coords.z = proj_coords.z - bias;

    float shadow = texture(shadow_map, proj_coords);

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

    vec4 frag_pos_light_space = s_light_matrix * vec4(world_pos, 1.0f);
    float shadow = shadowCalculation(frag_pos_light_space, normal, world_pos);

    light_info = (ambient + shadow * calcSpotLight(s_spot_light, normal, world_pos)) * albedo;
}