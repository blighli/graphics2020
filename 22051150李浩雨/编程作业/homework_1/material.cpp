#include "Material.h"

Material::Material(Shader* _shader, glm::vec3 diffuse, glm::vec3 specular, float shininess)
{
	this->shader = _shader;
	this->singleDiff = diffuse;
	this->singleSpec = specular;
	this->shininess = shininess;
	this->shineStrenth = 1.0f;
}

Material::Material(Shader* _shader, unsigned int _diffuse, unsigned int _specular, float shininess)
{
	this->shader = _shader;
	this->diffuse = _diffuse;
	this->specular = _specular;
	this->shininess = shininess;
	this->shineStrenth = 1.0f;
}

void Material::SetUniform4f(const char* paramNameString, glm::vec4 param)
{
	glUniform4f(glGetUniformLocation(shader->ID, paramNameString), param.x, param.y, param.z, param.w);
}

void Material::SetUniform3f(const char* paramNameString, glm::vec3 param)
{
	glUniform3f(glGetUniformLocation(shader->ID, paramNameString), param.x, param.y, param.z);
}

void Material::SetUniform1f(const char* paramNameString, float param)
{
	glUniform1f(glGetUniformLocation(shader->ID, paramNameString), param);
}

void Material::SetUniform1i(const char* paramNameString, int slot)
{
	glUniform1i(glGetUniformLocation(shader->ID, paramNameString), slot);
}
