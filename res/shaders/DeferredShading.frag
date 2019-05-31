#version 450

layout(binding = 0) uniform sampler2D rt_positions;
layout(binding = 1) uniform sampler2D rt_normals;
layout(binding = 2) uniform sampler2D rt_albedo_specular;
layout(binding = 3) uniform sampler2D rt_depth;

uniform float specular_intensity;
uniform float specular_power;
uniform vec3 g_cam_pos;

in vec2 texcoord;
out vec4 frag_color;

void main()
{
	vec3 normal      = texture(rt_normals, texcoord).rgb;
	vec3 world_pos   = texture(rt_positions, texcoord).rgb;
	vec4 albedo_spec = texture(rt_albedo_specular, texcoord);

	vec3 direction = normalize(vec3(-1, -1, 0));

    float diffuse = max(dot(normal, -direction), 0.0f);

    vec3 dir_to_eye  = normalize(g_cam_pos - world_pos);
    vec3 half_vector = normalize(dir_to_eye - direction);
    float specular   = pow(max(dot(half_vector, normal), 0.0f), specular_power);

    vec4 diffuse_color  = vec4(diffuse * albedo_spec.rgb, 1.0f);
    vec4 specular_color = vec4(vec3(specular) * albedo_spec.a, 1.0f);

	frag_color = diffuse_color + specular_color;

	gl_FragDepth = texture(rt_depth, texcoord).r;
}