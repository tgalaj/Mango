#version 450

in vec2 texcoord;
out vec4 fragColor;

layout(binding = 0) uniform sampler2D filterTexture;

const float gamma    = 1.6f;
const float exposure = 1.0f;

void main()
{
    vec3 ldr_color = texture(filterTexture, texcoord).rgb;

    /* Exposure tone mapping */
    vec3 hdr_color = vec3(1.0f) - exp(-ldr_color * exposure);

    /* Gamma correction */
    hdr_color = pow(hdr_color, vec3(1.0f / gamma));

	fragColor = vec4(hdr_color, 1.0f);
}