#version 460 core
in vec2 TexCoord;

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texTure1;
uniform sampler2D texTure2;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

void main()
{
	//ambient
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;

	//diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm,lightDir),0.0);
	vec3 diffuse = diff * lightColor;

	FragColor = mix(texture(texTure1, TexCoord),texture(texTure2, TexCoord),0) * vec4((ambient + diffuse),0.2);
}