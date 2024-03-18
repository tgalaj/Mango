#version 450

in vec2 texcoord;
out vec4 fragColor;

layout(binding = 0) uniform sampler2D filterTexture;

void main()
{
	fragColor = texture(filterTexture, texcoord);
}