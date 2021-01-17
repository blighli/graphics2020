// Test.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "SOIL/SOIL.h"
#include <iostream>
using namespace std;
#include <windows.h>
#include <gl/glew.h>
#include <gl/glut.h>
#include<gl/glew.h>

//glm库
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

//// Shaders
//const GLchar* vertexShaderSource = "#version 330 core\n"
//"layout (location = 0) in vec3 position;\n"
//"layout (location = 1) in vec3 color;\n"
//"layout (location = 2) in vec2 texCoor;\n"
//"out vec2 TexCoords;\n"
//"out vec3 outColor;\n"
//"uniform mat4 transform;\n"
//"void main()\n"
//"{\n"
//"gl_Position =transform*vec4(position, 1.0);\n"
//"TexCoords = texCoor;    \n"
//"outColor = color;    \n"
//"}\n\0";
//
//const GLchar* fragmentShaderSource = "#version 330 core\n"
//"in vec2 TexCoords;\n"
//"in vec3 outColor;\n"
//"layout (location = 0) out vec4  color;\n"
//"uniform sampler2D texture1; \n"
//"uniform sampler2D texture2; \n"
//"void main()\n"
//"{\n"  
//"color =mix(texture(texture1, TexCoords), texture(texture2, TexCoords), 0.4);\n" 
////"color =texture(texture1, TexCoords)* vec4(outColor, 0.8f);\n"
//"}\n\0";

// 立方体的8个顶点坐标
GLfloat cubeVertices[] = {
	// Positions		//Colors  			// Texture Coords
	-0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
	0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
	0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
	0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 0.0f, 0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 0.0f,
	0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 0.0f,1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 0.0f,1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 0.0f, 0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

	0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 0.0f,1.0f, 0.0f,
	0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
	0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 0.0f,
	0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
	0.5f, -0.5f,  0.5f,    1.0f, 1.0f, 0.0f,1.0f, 0.0f,
	0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 0.0f, 0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 0.0f,1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 0.0f,1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f, 0.0f, 1.0f
};


GLuint loadTexture(const char* path)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return textureID;
}




GLuint VBO, VAO;
GLuint faceTexture;
GLuint backTexture;

//GLuint shaderProgram;
Shader* shaderFile;
void  InitShaders()
{
	shaderFile = new Shader("vertShader.vert", "fragShader.frag");
}

float  x, y, z;
float angleX, angleY, angleZ;
float scaleX, scaleY, scaleZ;
glm::vec3 cameraPos;
glm::vec3 cameraFront;
glm::vec3 cameraUp;
void InitData()
{
	x = y = 0.0;
	z = -10;
	angleX = angleY = angleZ = 0;
	scaleX = scaleY = scaleZ = 0.5;
	cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	faceTexture = loadTexture("smileface.png");
	backTexture = loadTexture("map.png");
}


void SetupRC()
{
	// Black background
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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
		cameraPos.z -= 0.1;
		break;
	case 's':
		cameraPos.z += 0.1;
		break;
	case 'a':
		cameraPos.x -= 0.1;
		break;
	case 'd':
		cameraPos.x += 0.1;
		break;
	case 'e':
		cameraPos.y += 0.1;
		break;
	case 'f':
		cameraPos.y -= 0.1;
		break;
	case 'x':
		angleX += 0.2;
		break;
	case 'y':
		angleY += 0.2;
		break;
	case 'z':
		angleZ += 0.2;
		break;
	}
}

void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shaderFile->UseShader();
	glm::mat4 transform;
	transform = glm::translate(transform, glm::vec3(x, y, z));
	transform = glm::rotate(transform, angleY, glm::vec3(0.0f, 1.0f, 0.0f));
	transform = glm::rotate(transform, angleX, glm::vec3(1.0f, 0.0f, 0.0f));
	transform = glm::scale(transform, glm::vec3(scaleX, scaleY, scaleZ));
	angleY += 0.0005;
	angleX += 0.0002;
	glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	GLint transformLoc = glGetUniformLocation(shaderFile->GetShaderProgram(), "model");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	GLint viewLoc = glGetUniformLocation(shaderFile->GetShaderProgram(), "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glm::mat4 projection;
	projection = glm::perspective(45.0f, (GLfloat)800 / 600, 0.1f, 100.0f);
	GLint projectionLoc = glGetUniformLocation(shaderFile->GetShaderProgram(), "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, backTexture);
	glUniform1i(glGetUniformLocation(shaderFile->GetShaderProgram(), "texture1"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, faceTexture);
	glUniform1i(glGetUniformLocation(shaderFile->GetShaderProgram(), "texture2"), 1);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glutSwapBuffers();
}
void IdleFunc()
{
	glutPostRedisplay();
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
	glutIdleFunc(IdleFunc);
	SetupRC();
	glutMainLoop();
	return 0;
}



