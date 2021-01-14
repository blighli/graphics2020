#include "light.h"

DirectionalLight::DirectionalLight(int _type, glm::vec3 _angleOrDir, glm::vec3 _color, float _intensity, glm::vec3 _position)
{
	this->color = _color;
	this->intensity = _intensity;
	this->position = _position;
	this->angle = _angleOrDir;
	this->direction = _angleOrDir;
	UpdateAngleOrDirection(_type);
}

void DirectionalLight::UpdateAngleOrDirection(int type)
{
	if (type == ANGLE) {
		glm::vec3 direction = glm::vec3(0.0f, 0.0f, 1.0f);//like unity
		direction = glm::rotateZ(direction, angle.z);
		direction = glm::rotateX(direction, angle.x);
		direction = glm::rotateY(direction, angle.y);
		this->direction = direction;
	}
	else {
		glm::vec3 angle(0.0f);
		angle.x = glm::asin(direction.y);
		if (direction.z > 0) {
			angle.y = glm::asin(direction.x / glm::cos(glm::asin(direction.y)));
		}
		else {
			angle.y = glm::acos(-1) - glm::asin(direction.x / glm::cos(glm::asin(direction.y)));
		}
		this->angle = angle;
	}
}