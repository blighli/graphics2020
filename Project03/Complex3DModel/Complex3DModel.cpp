// Test.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Shader.h"
#include "model.h"
#include "Camera.h"
#include "mesh.h"

#include <iostream>
using namespace std;
#include <windows.h>
#include <gl/glew.h>
#include <gl/glut.h>

//glm库
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


Shader* shaderFile;
Model* model;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

void  InitShaders()
{
	shaderFile = new Shader("vertShader.vert", "fragShader.frag");
}
float  x, y, z;
float angleX, angleY, angleZ;
float scaleX, scaleY, scaleZ;
void InitData()
{
	x = y = 0.0;
	z = -10;
	angleX = angleY = angleZ = 0;
	scaleX = scaleY = scaleZ = 0.5;
	model = new Model("../bin/model/arakkoa_warrior.obj");
}


void SetupRC()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	InitData();
	InitShaders();
}

void KeyboardFunc(unsigned char key, int posx, int posy)
{
	switch (key)
	{
	case 'w':
		camera.ProcessKeyboard(Camera::FORWARD);
		break;
	case 's':
		camera.ProcessKeyboard(Camera::BACKWARD);
		break;
	case 'a':
		camera.ProcessKeyboard(Camera::LEFT);
		break;
	case 'd':
		camera.ProcessKeyboard(Camera::RIGHT);
		break;
	}
}

void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shaderFile->UseShader();
	glm::mat4 view = camera.GetViewMatrix();
	GLint viewLoc = glGetUniformLocation(shaderFile->GetShaderProgram(), "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glm::mat4 projection;
	projection = camera.GetProjectionMatrix(45.0f, (GLfloat)800 / 600, 0.1f, 100.0f);
	GLint projectionLoc = glGetUniformLocation(shaderFile->GetShaderProgram(), "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glm::mat4 modelMatrix;
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -1.74f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderFile->GetShaderProgram(), "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	model->Draw(*shaderFile);
	glutSwapBuffers();
}
void IdleFunc()
{
	glutPostRedisplay();
}
float lastPosX;
float lastPosY;
void MouseFunc(int button, int state, int x, int y)
{
	switch (state)
	{
	case GLUT_UP:
		switch (button)
		{
		case GLUT_WHEEL_UP:
			camera.ProcessKeyboard(Camera::FORWARD);
			break;
		case GLUT_WHEEL_DOWN:
			camera.ProcessKeyboard(Camera::BACKWARD);
			break;
		case GLUT_RIGHT_BUTTON:
			float xoffset = x - lastPosX;
			float yoffset = y - lastPosY;
			camera.ProcessMouseMovement(xoffset, yoffset);
			break;
		}
		break;
	case GLUT_DOWN:
		if (button == GLUT_RIGHT_BUTTON)
		{
			lastPosX = x, lastPosY = y;
		}
		break;
	}
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(800, 600);
	glutCreateWindow("GL_Test");
	glewInit();
	GLenum err = glewInit();
	glutDisplayFunc(RenderScene);
	glutKeyboardFunc(KeyboardFunc);
	glutMouseFunc(MouseFunc);
	glutIdleFunc(IdleFunc);
	SetupRC();
	glutMainLoop();
	return 0;
}



