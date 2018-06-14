#version 450

in vec2 texcoord;
in vec3 world_pos;

in mat3 tbn;

out vec4 frag_color;

const float NEAR = 0.01f;
const float FAR  = 100.0f;

layout(binding = 0) uniform sampler2D m_texture_diffuse;
layout(binding = 2) uniform sampler2D m_texture_normal;

subroutine vec4 debugRendering();
layout(location = 0) subroutine uniform debugRendering debug_func;

layout(index = 0) subroutine(debugRendering) vec4 debugPositions()
{
    vec4 color = vec4(world_pos, 0.0f);
    return color;
}

layout(index = 1) subroutine(debugRendering) vec4 debugNormals()
{
    vec3 normal = texture(m_texture_normal, texcoord).rgb;
    normal = tbn * (normal * 2.0f - 1.0f);

    vec4 color = vec4(normal, 0.0f);
    return color;
}

layout(index = 2) subroutine(debugRendering) vec4 debugTexCoords()
{
    vec4 color = vec4(texcoord, 0.0f, 0.0f);
    return color;
}

layout(index = 3) subroutine(debugRendering) vec4 debugDiffuse()
{
    vec4 color = texture(m_texture_diffuse, texcoord);
    return color;
}

layout(index = 4) subroutine(debugRendering) vec4 debugDepth()
{
    float depth = gl_FragCoord.z;
    float z = 2.0f * depth - 1.0f;
    float c = (2.0f * NEAR) / (FAR + NEAR - z * (FAR - NEAR)); // convert to lienear values

    vec4 color = vec4(vec3(c), 0.0f);
    return color;
}

void main()
{
    frag_color = debug_func();
}