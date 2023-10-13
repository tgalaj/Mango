#version 450

in vec2 texcoord;
out vec4 fragColor;

layout(binding = 0) uniform sampler2D filterTexture;

const float NEAR = 1.0f;
const float FAR  = 500.0f;

subroutine vec4 debugRendering();
layout(location = 0) subroutine uniform debugRendering debug_func;

layout(index = 0) subroutine(debugRendering) vec4 debugColorTarget()
{
    vec4 color = texture(filterTexture, texcoord);
    return color;
}

float linearizeDepth(vec2 uv)
{
    float z = texture(filterTexture, uv).x;
    return (2.0 * NEAR) / (FAR + NEAR - z * (FAR - NEAR));
}

layout(index = 1) subroutine(debugRendering) vec4 debugDepthTarget()
{
    float c = linearizeDepth(texcoord);
    vec4 color = vec4(vec3(c), 1.0f);
    return color;
}

void main()
{
    fragColor = debug_func();
}