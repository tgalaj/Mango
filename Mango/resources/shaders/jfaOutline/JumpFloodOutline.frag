#version 460 core

#define JFA_NULL_POS vec2(-1, -1)

layout(binding = 0) uniform sampler2D jfa_tex;

layout(location = 0) in vec2 uv;

out vec4 frag_color;

uniform vec3 outline_color;
uniform float outline_width;

void main()
{
    vec2 closest_pos = texture(jfa_tex, uv).xy;

    if (closest_pos == JFA_NULL_POS)
    {
        frag_color = vec4(0);
        return;
    }

    float dist = length(closest_pos - uv);

    // calculate outline
    // + 1.0 is because encoded nearest position is half a pixel inset
    // not + 0.5 because we want the anti-aliased edge to be aligned between pixels
    // distance is already in pixels, so this is already perfectly anti-aliased!
    float outline = clamp(outline_width - dist + 1.0, 0.0, 1.0);

    frag_color    = vec4(outline_color, 1.0);
    frag_color.a *= outline;
}