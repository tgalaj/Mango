#version 450

in vec2 texcoord;
in vec3 world_pos;

in mat3 tbn;

out vec4 frag_color;

layout(binding = 0) uniform sampler2D m_texture_diffuse;
layout(binding = 4) uniform sampler2D m_texture_depth;

uniform vec3 s_scene_ambient;
uniform vec3 g_cam_pos;
uniform float alpha_cutoff;

#include "ParallaxMapping.glh"

void main()
{
    vec3 dir_to_eye = normalize(g_cam_pos - world_pos) * tbn;
    vec2 parallax_texcoord = parallaxMapping(dir_to_eye);
	vec4 texture_color = texture(m_texture_diffuse, parallax_texcoord);

	if(texture_color.a < alpha_cutoff)
	{
		discard;
	}

    frag_color = texture_color * vec4(s_scene_ambient, 1.0f);
}