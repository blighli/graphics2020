#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

class Material
{
public:
	Material(Shader* _shader, glm::vec3 diffuse, glm::vec3 specular, float shininess);
	Material(Shader* _shader, unsigned int _diffuse, unsigned int _specular, float shininess);

	void SetUniform4f(const char* paramNameString, glm::vec4 param);
	void SetUniform3f(const char* paramNameString, glm::vec3 param);
	void SetUniform1f(const char* paramNameString, float param);
	void SetUniform1i(const char* paramNameString, int slot);

	enum slot {
		texture_diffuse,
		texture_specular
	};

	Shader* shader;
	unsigned int diffuse;
	unsigned int specular;
	glm::vec3 singleDiff;
	glm::vec3 singleSpec;
	float shininess;
	float shineStrenth;
};

#endif // !_MATERIAL_H_
