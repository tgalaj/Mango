#VS

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

out vec3 o_position;
out vec3 o_normal;
out vec2 o_texcoord;

uniform mat4 view;
uniform mat4 viewProj;

void main()
{
    o_normal   = mat3(view) * normal;
	o_position = vec3(view * vec4(position, 1.0f));
    o_texcoord = texcoord;

    gl_Position = viewProj * vec4(position, 1.0f);
}

#FS

in vec3 o_position;
in vec3 o_normal;
in vec2 o_texcoord;

out vec4 fragColor;

vec4 lightPos       = vec4(0.0f, 10.0f, -10.0f, 1.0f);
vec3 lightIntensity = vec3(1.0f);
vec3 Kd             = vec3(1.0f);
vec3 Ka             = vec3(1.0f);
vec3 Ks             = vec3(1.0f);
float shininess     = 20.0f;

vec3 ads()
{
	vec3 n = normalize(o_normal);
	vec3 s = normalize(lightPos.xyz - o_position);
	vec3 v = normalize(-o_position);
	vec3 h = normalize(v + s);

	return lightIntensity * (Ka + Kd * max(dot(s, n), 0.0f) + Ks * pow(max(dot(h, n), 0.0f), shininess));
}

void main()
{      
	fragColor = vec4(ads() * vec3(0.9, 0.2, 0.1), 1.0f);
}