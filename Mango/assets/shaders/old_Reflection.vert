layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec3 o_position;
out vec3 o_normal;

uniform mat4 world;
uniform mat4 viewProj;
uniform mat3 normalMatrix;

void main()
{
	o_position = vec3(world * vec4(position, 1));
	o_normal   = normalMatrix * normal;
	
	gl_Position = viewProj * world * vec4(position, 1.0f);
}