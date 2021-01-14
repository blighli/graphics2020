#include "camera.h"
#include <cmath>

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp)
{
	this->Position = position;
	this->Forward = glm::normalize(target - position);
	this->Right = glm::normalize(glm::cross(Forward, worldUp));
	this->Up = glm::normalize(glm::cross(Right, Forward));
	this->WorldUp = glm::normalize(worldUp);
	this->Pitch = glm::radians(glm::asin(Forward.y));
	if (Forward.z > 0) {
		this->Yaw = glm::radians(glm::asin(Forward.x / glm::cos(glm::asin(Forward.y))));
	}
	else {
		this->Yaw = glm::radians(180.0f - glm::asin(Forward.x / glm::cos(glm::asin(Forward.y))));
	}
}

Camera::Camera(glm::vec3 position, float pitch, float yaw, glm::vec3 worldUp)
{
	this->Position = position;
	this->Forward = glm::vec3(glm::cos(pitch) * glm::sin(yaw), glm::sin(pitch), glm::cos(pitch) * glm::cos(yaw));
	this->Right = glm::normalize(glm::cross(Forward, worldUp));
	this->Up = glm::normalize(glm::cross(Right, Forward));
	this->WorldUp = glm::normalize(worldUp);
	this->Pitch = pitch;
	this->Yaw = yaw;
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(Position, Position + Forward, WorldUp);
}

void Camera::LookAt(glm::vec3 target)
{
	this->Forward = glm::normalize(target - this->Position);
	this->Right = glm::normalize(glm::cross(Forward, this->WorldUp));
	this->Up = glm::normalize(glm::cross(Right, Forward));
	this->Pitch = glm::asin(Forward.y);
	if (Forward.z > 0) {
		this->Yaw = glm::asin(Forward.x / glm::cos(glm::asin(Forward.y)));
	}
	else {
		this->Yaw = glm::acos(-1) - glm::asin(Forward.x / glm::cos(glm::asin(Forward.y)));
	}
}

void Camera::ProcessMouseMovement(float deltaX, float deltaY)
{
	this->Yaw -= deltaX * MouseSensitivity;
	this->Pitch -= deltaY * MouseSensitivity;

	constexpr double limit = glm::radians(89.0f);
	if (Pitch > limit)
		Pitch = limit;
	if (Pitch < -limit)
		Pitch = -limit;
	UpdateCameraVectors();
	//std::cout << "pitch: " << Pitch << "  yaw: " << Yaw << std::endl;
}

void Camera::ProcessKeyMovement(float deltaR, float deltaF, float deltaU)
{
	this->Position += this->Forward * deltaF * this->Speed;
	this->Position += this->Right * deltaR * this->Speed;
	this->Position += this->Up * deltaU * this->Speed;
}

void Camera::UpdateCameraVectors()
{
	this->Forward = glm::normalize(glm::vec3(glm::cos(Pitch) * glm::sin(Yaw), glm::sin(Pitch), glm::cos(Pitch) * glm::cos(Yaw)));
	this->Right = glm::normalize(glm::cross(Forward, WorldUp));
	this->Up = glm::normalize(glm::cross(Right, Forward));
}