#version 330 core
#define NR_POINT_LIGHTS 4
struct Material{
	sampler2D texture_diffuse1;
	sampler2D texture_diffuse2;
	sampler2D texture_diffuse3;
	sampler2D texture_specular1;
	sampler2D texture_specular2;
	float shininess;
};

struct DirLight{
	vec3 position;
	vec3 color;
	vec3 direction;
};

in vec2 TexCoord;
in vec3 worldPos;
in vec3 worldNormal;

uniform Material material;
uniform DirLight dirLight;
uniform vec3 ambientColor;
uniform vec3 viewPos;

out vec4 FragColor;

vec3 CalcDirLight(DirLight light, vec3 normalDir, vec3 viewDir);

void main()
{
	vec3 normalDir = normalize(worldNormal);
	vec3 viewDir = normalize(viewPos - worldPos);

	vec3 ambient = texture(material.texture_diffuse1, TexCoord).rgb * ambientColor;
	vec3 result = ambient;

	result += CalcDirLight(dirLight, normalDir, viewDir);

	FragColor = vec4(result, 1.0);
}



vec3 CalcDirLight(DirLight light, vec3 normalDir, vec3 viewDir){
	vec4 diffuseColor = texture(material.texture_diffuse1, TexCoord);
	vec4 specularColor = texture(material.texture_specular1, TexCoord);

	vec3 lightDir = normalize(-light.direction);	
	vec3 reflectDir = reflect(-lightDir, normalDir);

	vec3 diffuse = diffuseColor.rgb * light.color * max(dot(lightDir, normalDir),0.0);
	vec3 specular = specularColor.rgb * light.color * pow(max(dot(viewDir,reflectDir), 0.0), material.shininess);

	return diffuse + specular;
}