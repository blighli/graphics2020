#ifndef _LIGHT_H_
#define _LIGHT_H_
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

enum Type
{
	ANGLE,
	DIRECTION
};

class Light
{
public:
	glm::vec3 color;
	glm::vec3 position;
	float intensity;
};

class DirectionalLight : public Light {
public:
	DirectionalLight(int _type,
		glm::vec3 _angleOrDir,
		glm::vec3 _color = glm::vec3(1.0f),
		float _intensity = 1.0f,
		glm::vec3 _position = glm::vec3(0.0f));

	void UpdateAngleOrDirection(int type);

	glm::vec3 angle;
	glm::vec3 direction;
};

class PointLight : public Light {
public:
	PointLight(glm::vec3 _color = glm::vec3(1.0f),
		float _intensity = 1.0f,
		glm::vec3 _position = glm::vec3(0.0f),
		float _radius = 7.0f);

	float radius;
	float attenuation1;
	float attenuation2;
	float attenuation3;
};

#endif // !_LIGHT_H_
