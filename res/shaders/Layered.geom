layout(triangles, invocations = 6) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 mvps[6];

in vec3 v_position[];
in vec3 v_normal[];
in vec2 v_texcoord[];

out vec3 o_position;
out vec3 o_normal;
out vec2 o_texcoord;

void main()
{
	for(int i = 0; i < gl_in.length(); ++i)
	{
		o_position = v_position[i];
		o_normal   = v_normal[i];
		o_texcoord = v_texcoord[i];
		
		gl_Position = mvps[gl_InvocationID] * gl_in[i].gl_Position;
		gl_Layer = gl_InvocationID;
		EmitVertex();
	}
}