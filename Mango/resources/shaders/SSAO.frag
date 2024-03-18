#version 450

in vec2 texcoord;
layout(location = 0) out float fragColor;

layout(binding = 0) uniform sampler2D src_texture;
layout(binding = 1) uniform sampler2D gbuffer_positions;
layout(binding = 2) uniform sampler2D gbuffer_normals;
layout(binding = 3) uniform sampler2D noise_texture;

uniform vec3 samples[64];
uniform mat4 view;
uniform mat4 projection;

uniform int kernel_size;
uniform float radius;
uniform float bias;
uniform float power;

subroutine float ssaoRendering();
layout(location = 0) subroutine uniform ssaoRendering ssao_func;

layout(index = 0) subroutine(ssaoRendering) float calcSSAO()
{
    vec2 gbuffer_size   = textureSize(gbuffer_positions, 0);
    vec2 noise_tex_size = textureSize(noise_texture, 0);

    vec2 noise_scale = gbuffer_size / noise_tex_size;

    vec3 position          = vec3(view * vec4(texture(gbuffer_positions, texcoord).xyz, 1.0));
    vec3 normal            = vec3(transpose(inverse(view)) * vec4(texture(gbuffer_normals, texcoord).xyz, 0.0));
    vec3 rand_rotation_vec = normalize(texture(noise_texture, texcoord * noise_scale).xyz);

    vec3 tangent   = normalize(rand_rotation_vec - normal * dot(rand_rotation_vec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 tbn       = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for(int i = 0; i < kernel_size; ++i)
    {
        // get sample position
        vec3 samp = tbn * samples[i]; // from tangent to view-space
        samp = position + samp * radius;

        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(samp, 1.0);
        offset = projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0

        // get sample depth
        float sample_depth = (view * texture(gbuffer_positions, offset.xy)).z; // get depth value of kernel sample

        // range check & accumulate
        float range_check = smoothstep(0.0, 1.0, radius / abs(position.z - sample_depth));
        occlusion += (sample_depth >= samp.z + bias ? 1.0 : 0.0) * range_check;
    }

    return pow(1.0 - (occlusion / kernel_size), power);
}

layout(index = 1) subroutine(ssaoRendering) float blurSSAO()
{
    vec2 texel_size = 1.0 / vec2(textureSize(src_texture, 0));
    float result = 0.0;

    for(int x = -2; x < 2; ++x)
    {
        for (int y = -2; y < 2; ++y)
        {
            vec2 offset = vec2(float(x), float(y)) * texel_size;
            result += texture(src_texture, texcoord + offset).r;
        }
    }

    return result / (4.0 * 4.0);
}

void main()
{
    fragColor = ssao_func();
}