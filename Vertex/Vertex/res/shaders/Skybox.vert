layout(location = 0) in vec3 position;

out vec3 o_texcoord;

uniform mat4 wvp;

void main()
{
	o_texcoord = vec3(position.x, -position.yz);
	gl_Position = wvp * vec4(position, 1.0f);
}