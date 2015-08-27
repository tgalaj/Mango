//TODO: Lightning needs serious improvement in the future.

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

//Dir light properties
vec4 lightPos       = vec4(0.0f, 10.0f, -10.0f, 1.0f);
vec3 lightIntensity = vec3(1.0f);
vec3 Kd             = vec3(1.0f);
vec3 Ka             = vec3(1.0f);
vec3 Ks             = vec3(1.0f);
float shininess     = 20.0f;

//Spot light properties
vec3 spotPosition  = vec3(5.0f, 3.0f, 4.0f);
vec3 spotIntensity = vec3(1.0f);
vec3 spotDir       = vec3(0.0f) - spotPosition;
float spotExponent = 0.5f;
float cutoff       = 20.0f; //degrees

vec3 ads()
{
	vec3 n = normalize(o_normal);
	vec3 s = normalize(lightPos.xyz - o_position);
	vec3 v = normalize(-o_position);
	vec3 h = normalize(v + s);

	return lightIntensity * (Ka + Kd * max(dot(s, n), 0.0f) + Ks * pow(max(dot(h, n), 0.0f), shininess));
}

vec3 spotlightAds()
{
	spotDir = normalize(spotDir);

	vec3 s = normalize(spotPosition - o_position);
	float angle = acos(dot(-s, spotDir));
	cutoff = radians(clamp(cutoff, 0.0f, 90.0f));
	vec3 ambient = spotIntensity * Ka * vec3(0.1f, 0.1f, 0.1f);

	if(angle < cutoff)
	{
		float spotFactor = pow(dot(-s, spotDir), spotExponent);

		vec3 v = normalize(-o_position);
		vec3 h = normalize(v + s);

		return ambient + spotFactor * spotIntensity * (Kd * max(dot(s, o_normal), 0.0f) + Ks * pow(max(dot(h, o_normal), 0.0f), shininess));
	}
	else
	{
		return ambient;
	}
}

void main()
{      
	fragColor = vec4(ads() * vec3(o_texcoord, 0.0), 1.0f);
}