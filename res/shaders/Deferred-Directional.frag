#version 450

in vec2 texcoord;
#include "Deferred-Lighting.glh"

layout(binding = 5) uniform sampler2DShadow shadow_map;

uniform int pcf_kernel_size = 2;

uniform DirectionalLight s_directional_light;
uniform mat4 s_light_matrix;

float shadowCalculation(vec4 frag_pos_light_space, vec3 normal)
{
    vec3 proj_coords = frag_pos_light_space.xyz;  /* Perspective division not needed (using ortho matrix) */
    proj_coords      = proj_coords * 0.5f + 0.5f; /* Map from [-1, 1] to [0, 1] */

    if(proj_coords.z > 1.0f)
       return 0.0f;

    float bias = max(0.0001f * (1.0 - dot(normal, -s_directional_light.direction)), 0.00001f); /* Removes shadow acne artifact */
    proj_coords.z = proj_coords.z - bias;

    /* PCF filtering */
    float shadow = 0.0;
    float sampling_range = 0.5 * pcf_kernel_size - 0.5;
    vec3 shadow_map_texel_size = vec3(1.0 / textureSize(shadow_map, 0), 0.0);

    for(float x = -sampling_range; x <= sampling_range; x += 1.0f)
    {
        for(float y = -sampling_range; y <= sampling_range; y += 1.0f)
        {
            float pcf = texture(shadow_map, proj_coords.xyz + vec3(x, y, 0.0) * shadow_map_texel_size);
            shadow += pcf;
        }
    }

    return shadow / float(pcf_kernel_size * pcf_kernel_size);
}

void main()
{
    vec4 albedo    = vec4(texture(gbuffer_albedo_spec, texcoord).rgb, 1.0f);
    vec4 ambient   = vec4(s_scene_ambient * texture(ambient_occlusion_texture, texcoord).r, 1.0f);
    vec3 world_pos = texture(gbuffer_positions,   texcoord).xyz;
    vec3 normal    = texture(gbuffer_normals,     texcoord).xyz;

    vec4 frag_pos_light_space = s_light_matrix * vec4(world_pos, 1.0f);
    float shadow = shadowCalculation(frag_pos_light_space, normal);

    light_info = (ambient + shadow * calcDirectionalLight(s_directional_light, normal, world_pos)) * albedo;
}