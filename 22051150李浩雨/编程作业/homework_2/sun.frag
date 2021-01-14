#version 330 core
struct Material{
	sampler2D texture_diffuse1;
	sampler2D texture_diffuse2;
	sampler2D texture_diffuse3;
	sampler2D texture_specular1;
	sampler2D texture_specular2;
	float shininess;
};

in vec2 TexCoord;

uniform Material material;

out vec4 FragColor;

void main()
{
	vec3 result = texture(material.texture_diffuse1, TexCoord).xyz;
	FragColor = vec4(result, 1.0);
}