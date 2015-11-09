in vec3 o_position;
in vec3 o_normal;
in vec2 o_texcoord;

out vec4 fragColor;

struct Material
{
	sampler2D texture_diffuse;
	sampler2D texture_specular;
	sampler2D texture_normal;
};

uniform Material material;

//Dir light properties
vec4 lightPos       = vec4(10.0f, 50.0f, -10.0f, 1.0f);
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

	return lightIntensity * (Ka * texture(material.texture_diffuse, o_texcoord).xyz + Kd * texture(material.texture_diffuse, o_texcoord).xyz * max(dot(s, n), 0.0f) + texture(material.texture_specular, o_texcoord).xyz * Ks * pow(max(dot(h, n), 0.0f), shininess));
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
	fragColor = vec4(ads() , 1.0f);
}