#version 450

in vec2 texcoord;
out vec4 frag_color;

layout(binding = 0) uniform sampler2D m_texture;

uniform vec4 text_color;

void main()
{
	vec4 sampled = vec4(1.0f);
	sampled.w = texture(m_texture, texcoord).r;

    frag_color = text_color * sampled;
}