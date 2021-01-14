#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include "model.h"

class Camera
{
public:
	Camera(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp);
	Camera(glm::vec3 position, float pitch, float yaw, glm::vec3 worldUp);

	glm::vec3 Position;
	glm::vec3 Forward;
	glm::vec3 Right;
	glm::vec3 Up;
	glm::vec3 WorldUp;
	float Pitch;//¶¼ÊÇ»¡¶È
	float Yaw;
	float MouseSensitivity = 0.005f;
	float Speed = 0.01f;
	float Fov = 45.0f;

	glm::mat4 getViewMatrix();
	void LookAt(glm::vec3 pos);
	void ProcessMouseMovement(float deltaX, float deltaY);
	void ProcessKeyMovement(float deltaR, float deltaF, float deltaU);

	void printFrontModel(std::vector<Model>& models);

private:
	void UpdateCameraVectors();
};

#endif // !_CAMERA_H_
