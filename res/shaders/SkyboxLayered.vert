layout(location = 0) in vec3 position;

out vec3 v_texcoord;

uniform mat4 wvp;

void main()
{
	v_texcoord = vec3(position.x, -position.yz);
	gl_Position = wvp * vec4(position, 1.0f);
}