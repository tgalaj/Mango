#version 450

in vec2 texcoord;
out vec4 fragColor;

layout(binding = 0) uniform sampler2D filterTexture;
layout(binding = 1) uniform sampler2D bloomBlurTexture;

const float gamma    = 2.4f;
const float exposure = 1.0f;

void main()
{
    vec3 hdr_color   = texture(filterTexture, texcoord).rgb;
    vec3 bloom_color = texture(bloomBlurTexture, texcoord).rgb;

    hdr_color += bloom_color;

    /* Exposure tone mapping */
    vec3 result = vec3(1.0f) - exp(-hdr_color * exposure);
	
	/* Reinhard tone mapping */
	//vec3 result = hdr_color / (vec3(1.0f) + hdr_color);

    /* Gamma correction */
    result = pow(result, vec3(1.0f / gamma));

	fragColor = vec4(result, 1.0f);
}