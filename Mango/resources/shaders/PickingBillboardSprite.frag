#version 460 core

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

void main()
{
    if (gl_FragCoord.z <= data.depth)
    {
        data.id    = inID;
        data.depth = gl_FragCoord.z;
    }
}
