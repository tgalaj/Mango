#version 450
#include "Forward-Lighting.glh"
#include "ParallaxMapping.glh"

layout(binding = 5) uniform sampler2DShadow shadow_map;

uniform DirectionalLight s_directional_light;

float shadowCalculation(vec4 frag_pos_light_space, vec3 normal)
{
    vec3 proj_coords = frag_pos_light_space.xyz;// / frag_pos_light_space.w;
    proj_coords      = proj_coords * 0.5f + 0.5f; /* Map from [-1, 1] to [0, 1] */

    if(proj_coords.z > 1.0f)
        return 1.0f;

    float bias = max(0.0005f * (1.0 - dot(normal, -s_directional_light.direction)), 0.00005f); /* Removes shadow acne artifact */
    proj_coords.z = proj_coords.z - bias;

    float shadow = texture(shadow_map, proj_coords);

    return shadow;
}

void main()
{
    vec3 dir_to_eye = normalize(g_cam_pos - world_pos) * tbn;
    parallax_texcoord = parallaxMapping(dir_to_eye);

    vec4 diffuse_tex_color = texture(m_texture_diffuse, parallax_texcoord);
    
    vec3 normal = texture(m_texture_normal, parallax_texcoord).rgb;
    normal = normalize(tbn * (normal * 2.0f - 1.0f));

    float shadow = shadowCalculation(frag_pos_light_space, normal);

    frag_color = shadow * diffuse_tex_color * calcDirectionalLight(s_directional_light, normal, world_pos);
}