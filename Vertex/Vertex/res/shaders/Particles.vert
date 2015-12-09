layout(location = 0) in vec4 position;

uniform mat4 viewProj;

void main()
{
	gl_Position = viewProj * position;
}