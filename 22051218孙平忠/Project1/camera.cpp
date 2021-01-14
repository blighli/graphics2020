#include "camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp)//通过glm::lookAt使用相机功能
{
	Position = position;
	WorldUp = worldUp;
	ForWard = glm::normalize(target - position);//用目标点位置减去相机位置，再做一个归一化
	Right = glm::normalize(glm::cross(ForWard, WorldUp));//向量之间的叉乘得到的是垂直于该平面的向量，如x与z，得到的是y
	Up = glm::normalize(glm::cross(Right, ForWard));
	//GetViewMatrix();
}
Camera::Camera(glm::vec3 position, float pitch, float yaw, glm::vec3 worldUp) {
	Position = position;
	WorldUp = worldUp;
	Pitch = pitch;
	Yaw = yaw;
	//下面是通过俯仰角和偏航角来计算前方位置，记住在这个上，官方教程有误，按本教程写，想知道原理的看傅老师的camera（2）
	ForWard.x = sin(Yaw) * cos(Pitch);
	ForWard.z = cos(Yaw)*cos(Pitch);
	ForWard.y = sin(Pitch);

	ForWard = glm::normalize(ForWard);
	Right = glm::normalize(glm::cross(ForWard, WorldUp));
	Up = glm::normalize(glm::cross(Right, ForWard));
}
void Camera::CameraViewMove(float offsetx, float offsety)//相机视角的移动，通过鼠标上一坐标点与下一坐标点差值做比较所得
{
	Pitch -= offsety*Sensitivity;
	Yaw -= offsetx* Sensitivity;
	UpdateViewPosition();
}
void Camera::CameraPosMoveZ(float offsetz)//在模型z轴上进行移动
{
	Position += ForWard * offsetz*0.1f;
}
void Camera::CameraPosMoveX(float offsetx)//在模型x轴上进行移动
{

	Position += Right * offsetx * 0.1f;
}
glm::mat4 Camera::GetViewMatrix()//获取视角矩阵
{
	return glm::lookAt(Position, Position + ForWard, Up);
}
void Camera::UpdateViewPosition()//刷新视角位置
{
	ForWard.x = sin(Yaw) * cos(Pitch);
	ForWard.z = cos(Yaw) * cos(Pitch);
	ForWard.y = sin(Pitch);
	ForWard = glm::normalize(ForWard);
	Right = glm::normalize(glm::cross(ForWard, WorldUp));
	Up = glm::normalize(glm::cross(Right, ForWard));

}

