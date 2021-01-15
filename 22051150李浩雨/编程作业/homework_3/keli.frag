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

struct PointLight{
	vec3 position;
	vec3 color;
	float atten1;
	float atten2;
	float atten3;
};

struct SpotLight{
	vec3 position;
	vec3 color;
	vec3 direction;
	float cutOff;
	float outerCutOff;
};

in vec2 TexCoord;
in vec3 worldPos;
in vec3 worldNormal;

uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLight[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform vec3 ambientColor;
uniform vec3 viewPos;
uniform samplerCube skybox;

out vec4 FragColor;

vec3 CalcDirLight(DirLight light, vec3 normalDir, vec3 viewDir);
vec3 CalcPoiLight(PointLight light, vec3 normalDir, vec3 viewDir);
vec3 CalcSpoLight(SpotLight light, vec3 normalDir, vec3 viewDir);

void main()
{
	vec3 normalDir = normalize(worldNormal);
	vec3 viewDir = normalize(viewPos - worldPos);

	vec3 reflectDir = reflect(-viewDir, normalDir);
	vec3 reflectColor = texture(skybox, reflectDir).rgb;

	vec3 ambient = texture(material.texture_diffuse1, TexCoord).rgb * ambientColor;
	vec3 result = ambient;

	result += CalcDirLight(dirLight, normalDir, viewDir);
	for(int i = 0; i < NR_POINT_LIGHTS; i++){
		result += CalcPoiLight(pointLight[i], normalDir, viewDir);
	}
	result += CalcSpoLight(spotLight, normalDir, viewDir);
	
	vec3 final = mix(result, reflectColor, 0.2);
	FragColor = vec4( final, 1.0);
}



vec3 CalcDirLight(DirLight light, vec3 normalDir, vec3 viewDir){
	vec4 diffuseColor = texture(material.texture_specular1, TexCoord);
	vec4 specularColor = texture(material.texture_specular1, TexCoord);

	vec3 lightDir = normalize(-light.direction);	
	vec3 reflectDir = reflect(-lightDir, normalDir);

	vec3 diffuse = diffuseColor.rgb * light.color * max(dot(lightDir, normalDir),0.0);
	vec3 specular = specularColor.rgb * light.color * pow(max(dot(viewDir,reflectDir), 0.0), material.shininess);

	return diffuse + specular;
}

vec3 CalcPoiLight(PointLight light, vec3 normalDir, vec3 viewDir){
	vec4 diffuseColor = texture(material.texture_diffuse3, TexCoord);
	vec4 specularColor = texture(material.texture_specular1, TexCoord);

	vec3 lightDir = normalize(light.position - worldPos);	
	vec3 reflectDir = reflect(-lightDir, normalDir);
	
	vec3 diffuse = diffuseColor.rgb * light.color * max(dot(lightDir, normalDir),0.0);
	vec3 specular = specularColor.rgb * light.color * pow(max(dot(viewDir,reflectDir), 0.0), material.shininess);

	float dist = length(light.position - worldPos);
	float attenuation = 1.0 / (light.atten1 + dist * light.atten2 + dist * dist * light.atten3);

	return attenuation * (diffuse + specular);
}

vec3 CalcSpoLight(SpotLight light, vec3 normalDir, vec3 viewDir){
	vec4 diffuseColor = texture(material.texture_diffuse3, TexCoord);
	vec4 specularColor = texture(material.texture_specular1, TexCoord);

	vec3 lightDir = normalize(light.position - worldPos);	
	vec3 reflectDir = reflect(-lightDir, normalDir);

	vec3 diffuse = diffuseColor.rgb * light.color * max(dot(lightDir, normalDir),0.0);
	vec3 specular = specularColor.rgb * light.color * pow(max(dot(viewDir,reflectDir), 0.0), material.shininess);
		
	float theta = dot(-lightDir, normalize(light.direction));
	float coef = clamp((theta - light.outerCutOff) / (light.cutOff - light.outerCutOff), 0.0, 1.0);

	return coef * (diffuse + specular);
}