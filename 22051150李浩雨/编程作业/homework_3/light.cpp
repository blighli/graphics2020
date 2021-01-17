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

PointLight::PointLight(glm::vec3 _color, float _intensity, glm::vec3 _position, float _radius)
{
	this->color = _color;
	this->intensity = _intensity;
	this->position = _position;
	this->radius = _radius;
	this->attenuation1 = 1.0f;
	this->attenuation2 = 0.7f;
	this->attenuation3 = 1.8f;
	if (radius == 50.0f) {
		this->attenuation1 = 1.0f;
		this->attenuation2 = 0.09f;
		this->attenuation3 = 0.032f;
	}
}

SpotLight::SpotLight(int _type, glm::vec3 _angleOrDir, glm::vec3 _color, float _intensity, glm::vec3 _position, float _cutOff, float _outerCutOff)
{
	this->color = _color;
	this->intensity = _intensity;
	this->position = _position;
	this->angle = _angleOrDir;
	this->direction = _angleOrDir;
	UpdateAngleOrDirection(_type);
	this->cutOff = _cutOff;
	this->outerCutOff = _outerCutOff;
}

void SpotLight::UpdateAngleOrDirection(int type)
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

AreaLight::AreaLight(int _type, glm::vec3 _angleOrDir, glm::vec3 _color, float _intensity, glm::vec3 _position, float _width, float _height)
{
	this->color = _color;
	this->intensity = _intensity;
	this->position = _position;
	this->width = _width;
	this->height = _height;
	this->angle = _angleOrDir;
	this->direction = _angleOrDir;
	UpdateAngleOrDirection(_type);
	//this->point[0] = glm::vec4(position - right * (width / 2) + up * (height / 2), 1.0f);
	//this->point[1] = glm::vec4(position + right * (width / 2) + up * (height / 2), 1.0f);
	//this->point[2] = glm::vec4(position - right * (width / 2) - up * (height / 2), 1.0f);
	//this->point[3] = glm::vec4(position + right * (width / 2) - up * (height / 2), 1.0f);
}

void AreaLight::UpdateAngleOrDirection(int type)
{
	if (type == ANGLE) {
		glm::vec3 direction = glm::vec3(0.0f, 0.0f, 1.0f);//like unity
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		direction = glm::rotateZ(direction, angle.z);
		direction = glm::rotateX(direction, angle.x);
		direction = glm::rotateY(direction, angle.y);
		up = glm::rotateZ(up, angle.z);
		up = glm::rotateX(up, angle.x);
		up = glm::rotateY(up, angle.y);
		this->direction = direction;
		this->up = up;
		this->right = glm::normalize(glm::cross(direction, up));
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
		this->up = glm::vec3(0.0f, 1.0f, 0.0f);
		this->right = glm::normalize(glm::cross(this->direction, this->up));
	}
}