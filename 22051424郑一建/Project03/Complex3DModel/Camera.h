#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 0.05f;


class Camera
{
public:
	enum Dirction {FORWARD,BACKWARD,LEFT,RIGHT};
	glm::vec3 position;
	glm::vec3 forword;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;
	float yaw;
	float pitch;
	float speed;
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) 
		: forword(glm::vec3(0.0f, 0.0f, -1.0f)), speed(SPEED)
	{
		this->position = position;
		this->worldUp = up;
		this->yaw = yaw;
		this->pitch = pitch;
		updateCamera();
	}
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : forword(glm::vec3(0.0f, 0.0f, -1.0f)), speed(SPEED)
	{
		position = glm::vec3(posX, posY, posZ);
		worldUp = glm::vec3(upX, upY, upZ);
		yaw = yaw;
		pitch = pitch;
		updateCamera();
	}

	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(position, position + forword, up);
	}
	glm::mat4 GetProjectionMatrix(float fovy,float aspect,float zNear,float zFar)
	{
		return glm::perspective(fovy, aspect, zNear, zFar);
	}

	glm::mat4 GetProjectionMatrix(float left,float right,float bottom,float top,float zNear,float zFar)
	{
		return glm::ortho(left, right, bottom, top, zNear, zFar);
	}
	void ProcessKeyboard(Dirction direction)
	{
		switch (direction)
		{
		case Camera::FORWARD:
			position += forword * speed;
			break;
		case Camera::BACKWARD:
			position -= forword * speed;
			break;
		case Camera::LEFT:
			position -= right * speed;
			break;
		case Camera::RIGHT:
			position += right * speed;
			break;
		default:
			break;
		}
	}

	void ProcessMouseMovement(float xoffset, float yoffset, bool constrainpitch = true)
	{
		xoffset *= 0.1;
		yoffset *= 0.1;
		yaw += xoffset;
		pitch += yoffset;
		if (constrainpitch)
		{
			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;
		}
		updateCamera();
	}

	void updateCamera()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		forword = glm::normalize(front);
		right = glm::normalize(glm::cross(forword, worldUp));  
		up = glm::normalize(glm::cross(right, forword));
	}
};