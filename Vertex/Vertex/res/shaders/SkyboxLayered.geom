layout(triangles, invocations = 6) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 mvps[6];

in vec3 v_texcoord[];

out vec3 o_texcoord;

void main()
{
	for(int i = 0; i < gl_in.length(); ++i)
	{
		o_texcoord = v_texcoord[i];
		
		gl_Position = mvps[gl_InvocationID] * gl_in[i].gl_Position;
		gl_Layer = gl_InvocationID;
		EmitVertex();
	}
}