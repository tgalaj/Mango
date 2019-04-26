#version 450

in vec2 texcoord;
out vec4 frag_color;

layout(binding = 0) uniform sampler2D m_texture_diffuse;

void main()
{
    frag_color = texture(m_texture_diffuse, texcoord);
}