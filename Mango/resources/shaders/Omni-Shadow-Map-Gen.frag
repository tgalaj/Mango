#version 450

in vec4 world_pos;

uniform vec3  s_light_pos;
uniform float s_far_plane;

void main()
{
    float light_distance = length(world_pos.xyz - s_light_pos);

    /* Map to [0, 1] depth value by dividing by far_plane */
    light_distance = light_distance / s_far_plane;

    gl_FragDepth = light_distance;
}