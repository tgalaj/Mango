#version 450
#include "Deferred-Lighting.glh"

layout(binding = 5) uniform sampler2DShadow shadow_map;

uniform DirectionalLight s_directional_light;

float shadowCalculation(vec4 frag_pos_light_space, vec3 normal)
{
	vec3 proj_coords = frag_pos_light_space.xyz;// / frag_pos_light_space.w;
	proj_coords      = proj_coords * 0.5f + 0.5f; /* Map from [-1, 1] to [0, 1] */

	if(proj_coords.z > 1.0f)
	return 0.0f;

	float bias = max(0.0001f * (1.0 - dot(normal, -s_directional_light.direction)), 0.00001f); /* Removes shadow acne artifact */
	proj_coords.z = proj_coords.z - bias;

	float shadow = texture(shadow_map, proj_coords);

	return shadow;
}

void main()
{
	vec3 world_pos = texture(gbuffer_positions,   texcoord).xyz;
	vec3 normal    = texture(gbuffer_normals,     texcoord).xyz;
	vec3 albedo    = texture(gbuffer_albedo_spec, texcoord).rgb;

	//float shadow = shadowCalculation(frag_pos_light_space, normal);
	//frag_color = shadow * diffuse_tex_color * calcDirectionalLight(s_directional_light, normal, world_pos);

	//float diff = max(dot(normal, -s_directional_light.direction), 0.0f);
	light_info = calcDirectionalLight(s_directional_light, normal, world_pos);
}