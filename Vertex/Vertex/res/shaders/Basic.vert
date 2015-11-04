layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

out vec3 o_position;
out vec3 o_normal;
out vec2 o_texcoord;

uniform mat4 world;
uniform mat4 view;
uniform mat4 viewProj;

void main()
{
    o_normal   = mat3(view) * normal;
	o_position = vec3(view * vec4(position, 1.0f));
    o_texcoord = texcoord;

    gl_Position = viewProj * world * vec4(position, 1.0f);
}