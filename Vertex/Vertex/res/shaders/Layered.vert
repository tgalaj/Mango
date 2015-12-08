layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

out vec3 v_position;
out vec3 v_normal;
out vec2 v_texcoord;

//layout(std140) uniform TransformMatrices
//{
//	mat4 world;
//	mat4 viewProj;
//	mat3 normalMatrix;
//};
	
uniform mat4 world;
uniform mat4 viewProj;
uniform mat3 normalMatrix;
	
void main()
{
	v_position = vec3(world * vec4(position, 1.0f));
    v_normal   = mat3(normalMatrix) * normal;
    v_texcoord = texcoord;
	
    gl_Position = world * vec4(position, 1.0f);
}