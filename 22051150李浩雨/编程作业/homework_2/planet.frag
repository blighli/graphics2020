#version 330 core
#define NR_POINT_LIGHTS 1
struct Material{
	sampler2D texture_diffuse1;
	sampler2D texture_diffuse2;
	sampler2D texture_diffuse3;
	sampler2D texture_specular1;
	sampler2D texture_specular2;
	float shininess;
};

struct PointLight{
	vec3 position;
	vec3 color;
	float atten1;
	float atten2;
	float atten3;
};

in vec2 TexCoord;
in vec3 worldPos;
in vec3 worldNormal;

uniform Material material;
uniform PointLight pointLight[NR_POINT_LIGHTS];
uniform vec3 ambientColor;
uniform vec3 viewPos;

out vec4 FragColor;

vec3 CalcPoiLight(PointLight light, vec3 normalDir, vec3 viewDir);

void main()
{
	vec3 normalDir = normalize(worldNormal);
	vec3 viewDir = normalize(viewPos - worldPos);

	vec3 ambient = texture(material.texture_diffuse1, TexCoord).xyz * ambientColor;
	vec3 result = ambient;

	//result += CalcDirLight(dirLight, normalDir, viewDir);
	result += CalcPoiLight(pointLight[0], normalDir, viewDir);

	FragColor = vec4(result, 1.0);
}

vec3 CalcPoiLight(PointLight light, vec3 normalDir, vec3 viewDir){
	vec4 diffuseColor = texture(material.texture_diffuse1, TexCoord);
	vec4 specularColor = texture(material.texture_specular1, TexCoord);
	//vec4 diffuseColor = vec4(material.diffuse, 1.0);
	//vec4 specularColor = vec4(material.specular, 1.0);

	vec3 lightDir = normalize(light.position - worldPos);	
	vec3 reflectDir = reflect(-lightDir, normalDir);
	
	vec3 diffuse = diffuseColor.rgb * light.color * max(dot(lightDir, normalDir),0.0);
	vec3 specular = diffuseColor.rgb * light.color * pow(max(dot(viewDir,reflectDir), 0.0), material.shininess);

	float dist = length(light.position - worldPos);
	float attenuation = 1.0 / (light.atten1 + dist * light.atten2 + dist * dist * light.atten3);

	return attenuation * (diffuse + specular);
}