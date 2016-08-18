in vec3 o_position;
in vec3 o_normal;

out vec4 fragcolor;

uniform vec3 camPos;

layout(binding = 7) uniform samplerCube cubeSampler;

uniform mat4 world;

void main()
{
	vec3 n = normalize(o_normal);
	vec3 v = normalize(o_position - camPos);
	fragcolor = texture(cubeSampler, reflect(v, n));
}