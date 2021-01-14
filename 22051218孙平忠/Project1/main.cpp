#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include "camera.h"
#define GLFW_INCLUDE_GLU
#include<GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;
void ProcessInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//void key_callback(GLFWwindow *window, int key, int sccncode, int action, int mode);
// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 800;

//������ɫ��
const GLchar* vertexShaderSource = "#version 330 core\n"
"layout (location=0) in vec3 position;\n"
"void main()\n"
"{\n"
"gl_Position=vec4(position.x,position.y,position.z,1.0);\n"
"}\0";
//������ɫ��
const GLchar* fragmentShaderSource = "#version 330 core\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = vec4(1.0f, 0.4f, 0.2f, 1.0f);\n"
"}\n\0";
bool firstMouse = true;
float lastx = 300, lasty = 400;
Camera* myCamera = new Camera(glm::vec3(0, 0, 5.0f), glm::radians(-15.0f), glm::radians(180.0f), glm::vec3(0, 1.0f, 0));
int main()
{
	glfwInit();//��ʼ��
	// Set all the required options for GLFW 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//�������汾��
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//���ôΰ汾��
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions  ����һ������
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Openglhw", nullptr, nullptr);
	glfwMakeContextCurrent(window);//����Ϊ��ǰ����
	//�������
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//����ƶ�ʱ����mouse_callback����
	glfwSetCursorPosCallback(window, mouse_callback);
	// Set the required callback functions ���ûص�����
	//glfwSetKeyCallback(window, key_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);//���ô��ڵ�λ�úʹ�С
	//Ϊ����opengl�ܹ�ʹ��GLSL��д�Ķ�����ɫ�����룬���Ǳ���������ʱ��̬��������Դ�������������������Ǵ���һ����ɫ������
	//�ٴ���������ID���������Ǵ洢���������ɫ����GLuintȻ����glCreateShader������ɫ��
	GLuint vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//��һ���������ɫ��Դ�븽�ӵ���ɫ������Ȼ�������
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	/*glShaderSource��������ɫ��������Ϊ��һ�����������������ڶ�����ָ����Դ�����ж��ٸ��ַ���������ֻ��һ���������������Ƕ�����ɫ��������Դ�룬
	���ǿ��԰ѵ��ĸ���������ΪNULL��*/
	//����Ƿ����ɹ�
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	/*
	�������Ƕ���һ����������ʾ�Ƿ�ɹ����룬����Ҫһ�����������Ϣ������������еĻ�����Ȼ��������glGetShaderiv����Ƿ����ɹ��ˡ�
	�������ʧ�ܣ�����Ӧ����glGetShaderInfoLog��ȡ������Ϣ��Ȼ���ӡ��
	*/
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//����Ƭ����ɫ��
	GLuint fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	//������������ɫ������ǰ�������ɫ���������ӵ�һ����ɫ��������
	//��shader program������������Ⱦ��
	// Check for compile time errors  ����Ƿ񴴽��ɹ�
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//������������ɫ������ǰ�������ɫ���������ӵ�һ����ɫ��������
	//��shader program������������Ⱦ��

	//����һ���������
	GLuint shaderProgram;
	shaderProgram = glCreateProgram();
	//glCreateProgram��������һ�����򣬷����´����ĳ�������ID���á�
	//����������Ҫ��ǰ��������ɫ�����ӵ���������ϣ�Ȼ����glLinkProgram�������ǣ�
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);//����


	// Check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	//��������������ǿ��Ե���glUseProgram���������´����ĳ��������Ϊ�������������ܼ���������
	//����ɫ���������ӵ��������󣬲�Ҫ����ɾ����ɫ������
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//projMat = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.01f, 100.0f);

	// ��������İ˸����㱣�浽һ����������
	//GLfloat vertices[] = {
	//	-0.5f, -0.5f, -0.5f,
	//	0.5f, -0.5f, -0.5f,
	//	-0.5f, 0.5f, -0.5f,
	//	0.5f, 0.5f, -0.5f,
	//	-0.5f, -0.5f, 0.5f,
	//	0.5f, -0.5f, 0.5f,
	//	-0.5f, 0.5f, 0.5f,
	//	0.5f, 0.5f, 0.5f,
	//};
	GLfloat vertices[] = {
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, 0.5f, -0.5f,
		0.5f, 0.5f, -0.5f,
		-0.5f, 0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f, 0.5f,
		0.5f, -0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f,

		-0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, 0.5f,
		-0.5f, 0.5f, 0.5f,

		0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,

		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, 0.5f,
		0.5f, -0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, 0.5f, -0.5f,
		0.5f, 0.5f, -0.5f,
		0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f, -0.5f,
	};
	GLuint indices[] = { // ÿ�λ���һ�������� 
		0, 1, 2,    3, 1, 2,
		0, 2, 4,    6, 2, 4,
		0, 1, 4,    5, 1, 4,
		4, 5, 6,    7, 5, 6,
	    1, 3, 5,    7, 3, 5,
		2, 6, 3,    7, 3, 6,
	};
	//VBO�����㻺��������������һ����һ�޶���ID���������ǿ���ʹ��glGenBuffer��������һ������ID
	GLuint VBO;
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Ȼ�����ǿ��Ե���glBUffweData���������֮ǰ����Ķ������ݸ��Ƶ�������ڴ���
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//����һ��VAO(Vertex Array Object �����������)
	//����EBO(�����������)
	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	//����VAO�������������
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	//������Щ֪ʶ���ǾͿ��Ը���OpenGL��ν��Ͷ������ݣ�ÿһ����������ԣ�������ʹ��
	//glVertexAttripoint����������ö�������ָ��
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);//����
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered��ע�ᣩ VBO as the currently bound vertex buffer object so afterwards we can safely unbind�����

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs) ���VAO
	while (!glfwWindowShouldClose(window))
	{
		ProcessInput(window);
		glClearColor(0.2f, 0.4f, 0.3f, 1.0f);//��Ⱦ��ɫ����̨����
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);//������Ⱦ����

		glm::mat4 viewMat;
		viewMat = myCamera->GetViewMatrix();
		//for (int i = 0; i < 4; ++i)
		//{
		//	cout << transform[i].x << " " << transform[i].y << " " << transform[i].z << " " << transform[i].w << endl;
		//}

		unsigned int transformLoc = glGetUniformLocation(shaderProgram, "viewMat");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(viewMat));

		glBindVertexArray(VAO);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);//��
		//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		//glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
		myCamera -> UpdateViewPosition();
	}
	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}
// Is called whenever a key is pressed/released via GLFW
//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
//{
//	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//		glfwSetWindowShouldClose(window, GL_TRUE);
//}
void ProcessInput(GLFWwindow* window)//ͨ��������������������ƶ�
{

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		myCamera->CameraPosMoveZ(1);
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		myCamera->CameraPosMoveZ(-1);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		myCamera->CameraPosMoveX(1);
	}
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		myCamera->CameraPosMoveX(-1);
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)//������
{
	if (firstMouse == true)//��һ�ε��øú�����ʱ��ֱ�ӽ���ǰ�������ȥ����ֵ���Ѵ�����ֹһ��ʼ��ʱ������ӽǵľ޴�ζ�
	{
		lastx = xpos;
		lasty = ypos;
		firstMouse = false;
	}
	float offsetx = xpos - lastx;
	float offsety = ypos - lasty;
	//std::cout << offsetx << std::endl;
	lastx = xpos;
	lasty = ypos;
	myCamera->CameraViewMove(offsetx, offsety);//����CameraViewMove����
}

