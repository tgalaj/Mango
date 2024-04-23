#version 460 core

#define FLT_MAX 3.402823466e+38
#define JFA_NULL_POS vec2(-1, -1)

layout(binding = 0) uniform sampler2D jfa_tex;

layout(location = 0) in vec2 uv;

out vec4 frag_color;

uniform float step_width;

void main()
{
    ivec2 frag_coord   = ivec2(gl_FragCoord);
    vec2  texture_size = textureSize(jfa_tex, 0 /* lod */);

    float closest_distance = FLT_MAX;
    vec2  closest_coord    = vec2(0.0);

    vec2 offset_pos = JFA_NULL_POS;
    for (int u = -1; u <= 1; ++u)
    {
        for (int v = -1; v <= 1; ++v)
        {
            vec2 sample_coord = frag_coord + vec2(u, v) * step_width;
                 offset_pos   = texture(jfa_tex, sample_coord / texture_size).xy;

            vec2  displacement = frag_coord - offset_pos * texture_size;
            float distance     = dot(displacement, displacement);

            if (offset_pos != JFA_NULL_POS && distance < closest_distance)
            {
                closest_distance = distance;
                closest_coord    = offset_pos;
            }
        }
    }

    frag_color = vec4(closest_coord, 0.0, 1.0);
}