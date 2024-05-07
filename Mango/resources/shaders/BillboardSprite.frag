#version 460 core

layout(location = 0) out vec4 frag_color;

layout(binding = 0) uniform sampler2D sprite_tex;
uniform vec3 color;

in vec2 texcoords;

float rim_thickness = 3.0;
float rim_intensity = 1.0;

void main()
{
    vec2 texel_size = 1.0 / textureSize(sprite_tex, 0);
    vec2 size       = texel_size * rim_thickness;
    
    vec4 texel_color = texture(sprite_tex, texcoords);
    if (texel_color.a < 0.1)
        discard;

    float outline =  texture(sprite_tex, texcoords + vec2(-size.x,  0     )).a;
          outline *= texture(sprite_tex, texcoords + vec2( 0,       size.y)).a;
          outline *= texture(sprite_tex, texcoords + vec2( size.x,  0     )).a;
          outline *= texture(sprite_tex, texcoords + vec2( 0,      -size.y)).a;
        
    float rim_cap = outline * texel_color.a * rim_intensity;
    frag_color += rim_cap;    

    frag_color.rgb *= color;
    frag_color.a = 1;
}
