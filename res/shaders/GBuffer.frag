#version 450
layout (location = 0) out vec4 positions;
layout (location = 1) out vec4 texcoords;
layout (location = 2) out vec4 normals;
layout (location = 3) out vec4 albedo_specular;
layout (location = 4) out vec4 depth;

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

#include "ParallaxMapping.glh"

void main()
{
    vec3 dir_to_eye = normalize(g_cam_pos - world_pos) * tbn;
    parallax_texcoord = parallaxMapping(dir_to_eye);

    vec4 diffuse_tex_color = texture(m_texture_diffuse, texcoord);
    float specular         = texture(m_texture_specular, parallax_texcoord).r;

	if(diffuse_tex_color.a < alpha_cutoff)
	{
		discard;
	}

    vec3 normal = texture(m_texture_normal, parallax_texcoord).rgb;
    normal = normalize(tbn * (normal * 2.0f - 1.0f));

	positions       = vec4(world_pos, 1.0f);
	texcoords       = vec4(parallax_texcoord, 0.0f, 1.0f);
	normals         = vec4(normal, 1.0f);
	albedo_specular = diffuse_tex_color;//vec4(diffuse_tex_color.rgb, specular);
	depth           = vec4(gl_FragCoord.z, 0.0f, 0.0f, 0.0f);
}