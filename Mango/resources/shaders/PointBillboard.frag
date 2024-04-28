#version 460 core

layout(location = 0) out vec4 frag_color;

layout(binding = 0) uniform sampler2D sprite_tex;

in vec2 texcoords;

void main()
{
    frag_color = texture(sprite_tex, texcoords);
}