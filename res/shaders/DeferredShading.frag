#version 450

layout(binding = 0) uniform sampler2D rt_positions;
layout(binding = 1) uniform sampler2D rt_texcoords;
layout(binding = 2) uniform sampler2D rt_normals;
layout(binding = 3) uniform sampler2D rt_albedo_specular;
layout(binding = 4) uniform sampler2D rt_depth;

uniform vec3 g_cam_pos;

in vec2 texcoord;
out vec4 frag_color;

void main()
{
	frag_color = vec4(texture(rt_albedo_specular, texcoord).rgb, 1.0f);

	gl_FragDepth = texture(rt_depth, texcoord).r; // TODO: figure out better way to set depth
}