#version 460

struct PickingData
{
    int   id;
    float depth;
};

layout(std430, binding = 0) buffer PickingDataSSBO
{
    PickingData data;
};

layout(location = 0) in flat int inID;
layout(location = 1) in vec2 texcoord;

layout(binding = 0) uniform sampler2D m_texture_diffuse;

uniform float alpha_cutoff;

void main()
{
    vec4 texture_color = texture(m_texture_diffuse, texcoord);

    if (texture_color.a < alpha_cutoff)
    {
        discard;
    }

    if (gl_FragCoord.z < data.depth)
    {
        data.id    = inID;
        data.depth = gl_FragCoord.z;
    }
}