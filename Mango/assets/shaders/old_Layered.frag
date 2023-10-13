in vec3 o_position;
in vec3 o_normal;
in vec2 o_texcoord;

layout(location = 0) out vec4 fragColor;

uniform vec3 camPos;

void main()
{   
	vec3 normal = normalize(o_normal);
	vec3 viewDir = normalize(camPos - o_position);
	
	fragColor = vec4(o_position, 1.0f);
}