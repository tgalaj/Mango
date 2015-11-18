#define MAX_DIR_LIGHTS   5
#define MAX_POINT_LIGHTS 10
#define MAX_SPOT_LIGHTS  10

in vec3 o_position;
in vec3 o_normal;
in vec2 o_texcoord;

out vec4 fragColor;

struct Material
{
	sampler2D texture_diffuse;
	sampler2D texture_specular;
	sampler2D texture_normal;
	vec3 diffuseColor;
	float shininess;
};

uniform Material material;

struct DirLight
{
	vec3 direction;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform DirLight dirLights[MAX_DIR_LIGHTS];
uniform uint dirUsed;

struct PointLight
{
	vec3 position;
	vec3 attenuations;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform uint pointUsed;

struct SpotLight
{
	vec3 position;
	vec3 direction;
	vec3 attenuations;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	vec2 angles; //angles[0] - innerCutOff, angles[1] - outerCutOff
};

uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform uint spotUsed;

uniform vec3 camPos;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	
	/* Diffuse shading */
	float diff = max(dot(normal, lightDir), 0.0f);
	
	/* Specular shading */
	vec3 reflectedDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectedDir), 0.0f), material.shininess);
	
	/* Combine */
	vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse, o_texcoord));
	vec3 diffuse  = light.diffuse  * vec3(texture(material.texture_diffuse, o_texcoord))  * diff * material.diffuseColor;
	vec3 specular = light.specular * vec3(texture(material.texture_specular, o_texcoord)) * spec;
	
	return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 position, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - position);
	
	/* Diffuse shading */
	float diff = max(dot(normal, lightDir), 0.0f);
	
	/* Specular shading */
	vec3 reflectedDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectedDir), 0.0f), material.shininess);
	
	/* Attenuation */
	float dist = length(lightDir);
	float attenuation = 1.0f / (light.attenuations[0] + light.attenuations[1] * dist + light.attenuations[2] * dist * dist);
	
	/* Combine */
	vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse, o_texcoord));
	vec3 diffuse  = light.diffuse  * vec3(texture(material.texture_diffuse, o_texcoord))  * diff * material.diffuseColor;
	vec3 specular = light.specular * vec3(texture(material.texture_specular, o_texcoord)) * spec;
	
	return (ambient + diffuse + specular) * attenuation;
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 position, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - position);
	
	/* Diffuse shading */
	float diff = max(dot(normal, lightDir), 0.0f);
	
	/* Specular shading */
	vec3 reflectedDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectedDir), 0.0f), material.shininess);
	
	/* Attenuation */
	float dist = length(lightDir);
	float attenuation = 1.0f / (light.attenuations[0] + light.attenuations[1] * dist + light.attenuations[2] * dist * dist);
	
	/* Spotlight intensity */
	float theta     = dot(lightDir, normalize(-light.direction));
	float epsilon   = light.angles[0] - light.angles[1];
	float intensity = clamp((theta - light.angles[1]) / epsilon, 0.0f, 1.0f);
	
	/* Combine */
	vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse, o_texcoord));
	vec3 diffuse  = light.diffuse  * vec3(texture(material.texture_diffuse, o_texcoord))  * diff * material.diffuseColor;
	vec3 specular = light.specular * vec3(texture(material.texture_specular, o_texcoord)) * spec;
	
	return (ambient + diffuse + specular) * intensity * attenuation;
}

void main()
{   
	vec3 normal = normalize(o_normal);
	vec3 viewDir = normalize(camPos - o_position);
	
	vec3 finalColor = vec3(0.0f);
	
	/* Calculate Directional Lights */
	for(int i = 0; i < dirUsed; ++i)
	{
		finalColor += calcDirLight(dirLights[i], normal, viewDir);
	}
	
	/* Calculate Point Lights */
	for(int i = 0; i < pointUsed; ++i)
	{
		finalColor += calcPointLight(pointLights[i], normal, o_position, viewDir);
	}
	
	/* Calculate Spot Lights */
	for(int i = 0; i < spotUsed; ++i)
	{
		finalColor += calcSpotLight(spotLights[i], normal, o_position, viewDir);
	}
	
	fragColor = vec4(finalColor, 1.0f);
}