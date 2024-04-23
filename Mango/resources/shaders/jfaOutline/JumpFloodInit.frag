#version 460 core

#define JFA_NULL_POS vec2(-1, -1)
#define EPS 5e-3

layout(binding = 0) uniform sampler2D object_mask_tex;

layout(location = 0) in vec2 uv;

out vec4 frag_color;

void main()
{
    vec2 texelSize = 1.0 / textureSize(object_mask_tex, 0);
    mat3 values;
    for (int u = -1; u <= 1; ++u)
    {   
        for (int v = -1; v <= 1; ++v)
        {
            vec2 sample_uv = uv + texelSize * vec2(u, v);
            values[u][v] = texture(object_mask_tex, sample_uv).r;
        }
    }

    // interior return current uv
    if (values[1][1] > 0.99)
    {
        frag_color = vec4(uv, 0.0, 1.0);
        return;
    }

    // exterior, return null position
    if (values[1][1] < 0.01)
    {
        frag_color = vec4(JFA_NULL_POS, 0.0, 1.0);
        return;
    }

    // sobel to estimate the edge direction
    vec2 dir = -vec2(values[0][0] + values[0][1] * 2.0 + values[0][2] - values[2][0] - values[2][1] * 2.0 - values[2][2],
                     values[0][0] + values[1][0] * 2.0 + values[2][0] - values[0][2] - values[1][2] * 2.0 - values[2][2]);

    // if dir length is small, this is either a subpixel dor or line
    // no way to estimate subpixel's edge, output current uv (position)
    if (abs(dir.x) <= EPS && abs(dir.y) <= EPS)
    {
        frag_color = vec4(uv, 0.0, 1.0);
        return;
    }

    dir = normalize(dir);

    // subpixel offset
    vec2 offset = dir * (1.0 - values[1][1]);

    frag_color = vec4(uv + offset * texelSize, 0.0, 1.0);
}
