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

void main()
{
    vec4 texture_color = texture(m_texture_diffuse, texcoord);

    if (gl_FragCoord.z < data.depth)
    {
        data.id    = inID;
        data.depth = gl_FragCoord.z;
    }
}