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

//顶点着色器
const GLchar* vertexShaderSource = "#version 330 core\n"
"layout (location=0) in vec3 position;\n"
"void main()\n"
"{\n"
"gl_Position=vec4(position.x,position.y,position.z,1.0);\n"
"}\0";
//像素着色器
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
	glfwInit();//初始化
	// Set all the required options for GLFW 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//设置主版本号
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//设置次版本号
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions  创建一个窗口
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Openglhw", nullptr, nullptr);
	glfwMakeContextCurrent(window);//设置为当前窗口
	//隐藏鼠标
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//鼠标移动时呼叫mouse_callback函数
	glfwSetCursorPosCallback(window, mouse_callback);
	// Set the required callback functions 设置回调函数
	//glfwSetKeyCallback(window, key_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);//设置窗口的位置和大小
	//为了让opengl能够使用GLSL编写的顶点着色器代码，我们必须在运行时动态编译它的源码所以我们首先做的是创建一个着色器对象
	//再次引用它的ID，所以我们存储这个顶点着色器的GLuint然后用glCreateShader创建着色器
	GLuint vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//下一步是这个着色器源码附加到着色器对象然后编译它
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	/*glShaderSource函数把着色器对象作为第一个参数来编译它。第二参数指定了源码中有多少个字符串，这里只有一个。第三个参数是顶点着色器真正的源码，
	我们可以把第四个参数设置为NULL。*/
	//检查是否编译成功
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	/*
	首先我们定义一个整型来表示是否成功编译，还需要一个储存错误消息的容器（如果有的话）。然后我们用glGetShaderiv检查是否编译成功了。
	如果编译失败，我们应该用glGetShaderInfoLog获取错误消息，然后打印它
	*/
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//编译片段着色器
	GLuint fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	//编译完两个着色器后就是把两个着色器对象连接到一个着色器程序中
	//（shader program）它是用来渲染的
	// Check for compile time errors  检查是否创建成功
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//编译完两个着色器后就是把两个着色器对象连接到一个着色器程序中
	//（shader program）它是用来渲染的

	//创建一个程序对象
	GLuint shaderProgram;
	shaderProgram = glCreateProgram();
	//glCreateProgram函数创建一个程序，返回新创建的程序对象的ID引用。
	//现在我们需要把前面编译的着色器附加到程序对象上，然后用glLinkProgram链接它们：
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);//连接


	// Check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	//设置完上面后我们可以调用glUseProgram函数，用新创建的程序对象作为参数，这样就能激活程序对象
	//把着色器对象链接到程序对象后，不要忘记删除着色器对象
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//projMat = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.01f, 100.0f);

	// 将立方体的八个顶点保存到一个数组里面
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
	GLuint indices[] = { // 每次绘制一个三角形 
		0, 1, 2,    3, 1, 2,
		0, 2, 4,    6, 2, 4,
		0, 1, 4,    5, 1, 4,
		4, 5, 6,    7, 5, 6,
	    1, 3, 5,    7, 3, 5,
		2, 6, 3,    7, 3, 6,
	};
	//VBO（顶点缓冲对象）这个缓冲有一个独一无二的ID，所以我们可以使用glGenBuffer函数生成一个缓冲ID
	GLuint VBO;
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// 然后我们可以调用glBUffweData函数它会把之前定义的顶点数据复制到缓冲的内存中
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//生成一个VAO(Vertex Array Object 顶点数组对象)
	//建立EBO(索引缓冲对象)
	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	//建立VAO（顶点数组对象）
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	//有了这些知识我们就可以告诉OpenGL如何解释顶点数据（每一个顶点的属性），我们使用
	//glVertexAttripoint这个函数设置顶点属性指针
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);//开启
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered（注册） VBO as the currently bound vertex buffer object so afterwards we can safely unbind（解绑）

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs) 解绑VAO
	while (!glfwWindowShouldClose(window))
	{
		ProcessInput(window);
		glClearColor(0.2f, 0.4f, 0.3f, 1.0f);//渲染颜色到后台缓冲
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);//调用渲染程序

		glm::mat4 viewMat;
		viewMat = myCamera->GetViewMatrix();
		//for (int i = 0; i < 4; ++i)
		//{
		//	cout << transform[i].x << " " << transform[i].y << " " << transform[i].z << " " << transform[i].w << endl;
		//}

		unsigned int transformLoc = glGetUniformLocation(shaderProgram, "viewMat");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(viewMat));

		glBindVertexArray(VAO);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);//绑定
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
void ProcessInput(GLFWwindow* window)//通过监控鼠标来控制相机的移动
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

void mouse_callback(GLFWwindow* window, double xpos, double ypos)//检查鼠标
{
	if (firstMouse == true)//第一次调用该函数的时候直接将当前坐标给过去坐标值，已此来防止一开始的时候出现视角的巨大晃动
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
	myCamera->CameraViewMove(offsetx, offsety);//调用CameraViewMove函数
}

