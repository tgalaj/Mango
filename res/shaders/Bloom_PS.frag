#version 450

in vec2 texcoord;
layout(location = 0) out vec4 fragColor;

layout(binding = 0) uniform sampler2D srcTexture;

uniform float threshold;

float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

subroutine vec4 bloomRendering();
layout(location = 0) subroutine uniform bloomRendering bloom_func;

layout(index = 0) subroutine(bloomRendering) vec4 extractBrightness()
{
    vec3 color = texture(srcTexture, texcoord).rgb;
    vec3 luma  = vec3(0.2126, 0.7152, 0.0722);

    float brightness  = dot(color, luma);
    vec3 bright_color = step(threshold, brightness) * color;

    return vec4(bright_color, 1.0);
}

layout(index = 1) subroutine(bloomRendering) vec4 blurGaussianHorizontal()
{
    vec2 texel_size = 1.0 / textureSize(srcTexture, 0);
    vec3 result = texture(srcTexture, texcoord).rgb * weight[0];

    for(int i = 1; i < 5; ++i)
    {
        result += texture(srcTexture, texcoord + vec2(texel_size.x * i, 0.0)).rgb * weight[i];
        result += texture(srcTexture, texcoord - vec2(texel_size.x * i, 0.0)).rgb * weight[i];
    }

    return vec4(result, 1.0);
}

layout(index = 2) subroutine(bloomRendering) vec4 blurGaussianVertical()
{
    vec2 texel_size = 1.0 / textureSize(srcTexture, 0);
    vec3 result = texture(srcTexture, texcoord).rgb * weight[0];

    for(int i = 1; i < 5; ++i)
    {
        result += texture(srcTexture, texcoord + vec2(0.0, texel_size.y * i)).rgb * weight[i];
        result += texture(srcTexture, texcoord - vec2(0.0, texel_size.y * i)).rgb * weight[i];
    }

    return vec4(result, 1.0);
}

void main()
{
    fragColor = bloom_func();
}