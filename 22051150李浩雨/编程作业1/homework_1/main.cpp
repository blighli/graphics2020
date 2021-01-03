#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#define STB_IMAGE_IMPLEMENTATION

#include "camera.h"
#include "light.h"
#include "shader.h"
#include "material.h"
#include "model.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

Camera camera(glm::vec3(0.0f, 0.2f, 3.0f), glm::radians(0.0f), glm::radians(180.0f), glm::vec3(0, 1.0f, 0));

DirectionalLight dirLight = DirectionalLight(0,
	glm::vec3(glm::radians(45.0f), glm::radians(135.0f), 0.0f),
	glm::vec3(1.0f, 1.0f, 1.0f),
	1.0f,
	glm::vec3(-40.0f, 56.0f, 40.0f));

#pragma region InPut Declare
float lastX;
float lastY;
bool firstMouse = true;

void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
	if (firstMouse) {
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
		return;
	}
	float deltaX, deltaY;
	deltaX = xPos - lastX;
	deltaY = yPos - lastY;

	lastX = xPos;
	lastY = yPos;
	camera.ProcessMouseMovement(deltaX, deltaY);
	//cout << "deltax: " << deltaX << "    deltay: " << deltaY << endl;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	float fov = camera.Fov;
	if (fov >= 1.0f && fov <= 45.0f) {
		fov -= yoffset;
	}
	if (fov <= 1.0f) {
		fov = 1.0f;
	}
	if (fov >= 45.0f) {
		fov = 45.0f;
	}
	camera.Fov = fov;
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		camera.LookAt(glm::vec3(0, 0, 0));
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.ProcessKeyMovement(0.0f, 1.0f, 0.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeyMovement(0.0f, -1.0f, 0.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeyMovement(-1.0f, 0.0f, 0.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeyMovement(1.0f, 0.0f, 0.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		camera.ProcessKeyMovement(0.0f, 0.0f, 1.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		camera.ProcessKeyMovement(0.0f, 0.0f, -1.0f);
	}
}
#pragma endregion

int main(int argc, char* argv[]) {
	#pragma region Open a window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "homework_1", NULL, NULL);
	if (window == NULL) {
		std::cout << "open window failed." << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		std::cout << "glew init failed." << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glEnable(GL_DEPTH_TEST);
	#pragma endregion

	
	Shader* objShader = new Shader("obj.vert", "obj.frag");
	Material* objMat = new Material(objShader, glm::vec3(0.4f, 0.4f, 0.4f), glm::vec3(0.9f, 0.9f, 0.9f), 64.0f);

	std::string exePath = argv[0];
	Model cube(exePath.substr(0, exePath.find_last_of('\\')) + "\\model\\cube\\cube.obj");
	Model sphere(exePath.substr(0, exePath.find_last_of('\\')) + "\\model\\sphere\\sphere.obj");
	Model teapot(exePath.substr(0, exePath.find_last_of('\\')) + "\\model\\teapot\\teapot.obj");
	Model keli(exePath.substr(0, exePath.find_last_of('\\')) + "\\model\\keli\\keli.obj");

	glm::mat4 viewMat = glm::mat4(1.0f);
	glm::mat4 projMat = glm::mat4(1.0f);

	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		viewMat = camera.getViewMatrix();
		projMat = glm::perspective(glm::radians(camera.Fov), (float)800 / (float)600, 0.1f, 150.0f);

		cube.SetScale(0.2f, 0.2f, 0.2f);
		cube.SetRotation(0.0f, (float)glfwGetTime() * 30.0f, 0.0f);
		cube.SetPosition(0.0f, 0.1f, 1.0f);

		sphere.SetScale(0.2f, 0.2f, 0.2f);
		sphere.SetPosition(-0.5f, 0.1f, 1.0f);

		teapot.SetScale(0.2f, 0.2f, 0.2f);
		teapot.SetRotation(0.0f, (float)glfwGetTime() * 20.0f, 0.0f);
		teapot.SetPosition(1.0f, 0.1f, 1.0f);

		keli.SetScale(0.01f, 0.01f, 0.01f);
		keli.SetRotation(0.0f, (float)glfwGetTime() * 20.0f, 0.0f);
		keli.SetPosition(0.0f, -1.0f, -1.0f);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		objMat->shader->use();
		glUniformMatrix4fv(glGetUniformLocation(objMat->shader->ID, "viewMat"), 1, GL_FALSE, glm::value_ptr(viewMat));
		glUniformMatrix4fv(glGetUniformLocation(objMat->shader->ID, "projMat"), 1, GL_FALSE, glm::value_ptr(projMat));

		objMat->SetUniform3f("material.diffuse", objMat->singleDiff);
		objMat->SetUniform3f("material.specular", objMat->singleSpec);
		objMat->SetUniform1f("material.shininess", objMat->shininess);
		objMat->SetUniform3f("dirLight.position", dirLight.position);
		objMat->SetUniform3f("dirLight.color", dirLight.color);
		objMat->SetUniform3f("dirLight.direction", dirLight.direction);
		objMat->SetUniform3f("ambientColor", glm::vec3(0.1f, 0.1f, 0.1f));
		objMat->SetUniform3f("viewPos", camera.Position);
		glUniformMatrix4fv(glGetUniformLocation(objMat->shader->ID, "modelMat"), 1, GL_FALSE, glm::value_ptr(cube.GetModelMatrix()));
		cube.DrawAcMat(objMat);
		glUniformMatrix4fv(glGetUniformLocation(objMat->shader->ID, "modelMat"), 1, GL_FALSE, glm::value_ptr(sphere.GetModelMatrix()));
		sphere.DrawAcMat(objMat);
		glUniformMatrix4fv(glGetUniformLocation(objMat->shader->ID, "modelMat"), 1, GL_FALSE, glm::value_ptr(teapot.GetModelMatrix()));
		teapot.DrawAcMat(objMat);
		glUniformMatrix4fv(glGetUniformLocation(objMat->shader->ID, "modelMat"), 1, GL_FALSE, glm::value_ptr(keli.GetModelMatrix()));
		keli.DrawAcMat(objMat);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}