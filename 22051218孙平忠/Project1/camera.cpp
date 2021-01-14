#include "camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp)//ͨ��glm::lookAtʹ���������
{
	Position = position;
	WorldUp = worldUp;
	ForWard = glm::normalize(target - position);//��Ŀ���λ�ü�ȥ���λ�ã�����һ����һ��
	Right = glm::normalize(glm::cross(ForWard, WorldUp));//����֮��Ĳ�˵õ����Ǵ�ֱ�ڸ�ƽ�����������x��z���õ�����y
	Up = glm::normalize(glm::cross(Right, ForWard));
	//GetViewMatrix();
}
Camera::Camera(glm::vec3 position, float pitch, float yaw, glm::vec3 worldUp) {
	Position = position;
	WorldUp = worldUp;
	Pitch = pitch;
	Yaw = yaw;
	//������ͨ�������Ǻ�ƫ����������ǰ��λ�ã���ס������ϣ��ٷ��̳����󣬰����̳�д����֪��ԭ��Ŀ�����ʦ��camera��2��
	ForWard.x = sin(Yaw) * cos(Pitch);
	ForWard.z = cos(Yaw)*cos(Pitch);
	ForWard.y = sin(Pitch);

	ForWard = glm::normalize(ForWard);
	Right = glm::normalize(glm::cross(ForWard, WorldUp));
	Up = glm::normalize(glm::cross(Right, ForWard));
}
void Camera::CameraViewMove(float offsetx, float offsety)//����ӽǵ��ƶ���ͨ�������һ���������һ������ֵ���Ƚ�����
{
	Pitch -= offsety*Sensitivity;
	Yaw -= offsetx* Sensitivity;
	UpdateViewPosition();
}
void Camera::CameraPosMoveZ(float offsetz)//��ģ��z���Ͻ����ƶ�
{
	Position += ForWard * offsetz*0.1f;
}
void Camera::CameraPosMoveX(float offsetx)//��ģ��x���Ͻ����ƶ�
{

	Position += Right * offsetx * 0.1f;
}
glm::mat4 Camera::GetViewMatrix()//��ȡ�ӽǾ���
{
	return glm::lookAt(Position, Position + ForWard, Up);
}
void Camera::UpdateViewPosition()//ˢ���ӽ�λ��
{
	ForWard.x = sin(Yaw) * cos(Pitch);
	ForWard.z = cos(Yaw) * cos(Pitch);
	ForWard.y = sin(Pitch);
	ForWard = glm::normalize(ForWard);
	Right = glm::normalize(glm::cross(ForWard, WorldUp));
	Up = glm::normalize(glm::cross(Right, ForWard));

}

