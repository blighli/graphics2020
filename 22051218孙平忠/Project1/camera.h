#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
class Camera
{
public:
	Camera(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp);//通过目标点来控制视角
	Camera(glm::vec3 position, float pitch, float yaw, glm::vec3 worldUp);//通过欧拉角控制视角
	glm::vec3 Position;//相机位置
	glm::vec3 WorldUp;//世界坐标的竖直方向
	glm::vec3 ForWard;//模型前方
	glm::vec3 Right;//模型右方
	glm::vec3 Up;//模型上方
	float Pitch;//俯仰角
	float Yaw;//偏航角
	float Sensitivity = 0.01f;
	void CameraViewMove(float offsetx, float offsety);//相机视野的移动
	void CameraPosMoveZ(float offsetz);//相机位置Z轴上的移动
	void CameraPosMoveX(float offsetx);//相机位置X轴上的移动

	glm::mat4 GetViewMatrix();//获取视角矩阵
	void UpdateViewPosition();//刷新相机位置
};
