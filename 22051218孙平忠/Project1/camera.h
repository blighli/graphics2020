#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
class Camera
{
public:
	Camera(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp);//ͨ��Ŀ����������ӽ�
	Camera(glm::vec3 position, float pitch, float yaw, glm::vec3 worldUp);//ͨ��ŷ���ǿ����ӽ�
	glm::vec3 Position;//���λ��
	glm::vec3 WorldUp;//�����������ֱ����
	glm::vec3 ForWard;//ģ��ǰ��
	glm::vec3 Right;//ģ���ҷ�
	glm::vec3 Up;//ģ���Ϸ�
	float Pitch;//������
	float Yaw;//ƫ����
	float Sensitivity = 0.01f;
	void CameraViewMove(float offsetx, float offsety);//�����Ұ���ƶ�
	void CameraPosMoveZ(float offsetz);//���λ��Z���ϵ��ƶ�
	void CameraPosMoveX(float offsetx);//���λ��X���ϵ��ƶ�

	glm::mat4 GetViewMatrix();//��ȡ�ӽǾ���
	void UpdateViewPosition();//ˢ�����λ��
};
