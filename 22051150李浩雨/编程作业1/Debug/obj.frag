#version 330 core
#define NR_POINT_LIGHTS 4
struct Material{
	vec3 diffuse;
	vec3 specular;
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

	vec3 ambient = material.diffuse * ambientColor;
	vec3 result = ambient;

	result += CalcDirLight(dirLight, normalDir, viewDir);

	FragColor = vec4(result, 1.0);
}



vec3 CalcDirLight(DirLight light, vec3 normalDir, vec3 viewDir){
	vec4 diffuseColor = vec4(material.diffuse, 1.0);
	vec4 specularColor = vec4(material.specular, 1.0);

	vec3 lightDir = normalize(-light.direction);	
	vec3 reflectDir = reflect(-lightDir, normalDir);

	vec3 diffuse = diffuseColor.rgb * light.color * max(dot(lightDir, normalDir),0.0);
	vec3 specular = specularColor.rgb * light.color * pow(max(dot(viewDir,reflectDir), 0.0), material.shininess);

	return diffuse + specular;
}