#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#define STB_IMAGE_IMPLEMENTATION

#include <vector>
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

PointLight poiLight = PointLight(glm::vec3(0.8f, 0.5f, 0.0f),
	10.0f,
	glm::vec3(0.0f, 0.0f, 0.0f), 50.0f);

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

void processInput(GLFWwindow* window, std::vector<Model> models) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		camera.LookAt(glm::vec3(0, 0, 0));
	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		camera.printFrontModel(models);
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

#pragma region Skybox Vertices

float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

#pragma endregion

unsigned int LoadCubeMap(std::string path) {
	vector<std::string> faces
	{
		"right.jpg",
		"left.jpg",
		"top.jpg",
		"bottom.jpg",
		"front.jpg",
		"back.jpg"
	};

	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++) {

		string filename = path + '\\' + faces[i];

		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else {
			std::cout << "Cubemap texture failed to load at path: " << filename << std::endl;
		}
		stbi_image_free(data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

int main(int argc, char* argv[]) {
	#pragma region Open a window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "homework_2", NULL, NULL);
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

	Shader* sunShader = new Shader("sun.vert", "sun.frag");
	Shader* planetShader = new Shader("planet.vert", "planet.frag");
	Shader* skyboxShader = new Shader("skybox.vert", "skybox.frag");

	Material* sunMat = new Material(sunShader, 0, 0, 64.0f);
	Material* planetMat = new Material(planetShader, 0, 0, 32.0f);
	Material* skyboxMat = new Material(skyboxShader, 0, 0, 64.0f);
	

	#pragma region Model definition
	std::string exePath = argv[0];

	unsigned int cubemapTexture = LoadCubeMap(exePath.substr(0, exePath.find_last_of('\\')) + "\\skybox\\universe");
	Mesh skybox(skyboxVertices);

	Model sun("sun", exePath.substr(0, exePath.find_last_of('\\')) + "\\model\\sun and planets\\sun.obj");
	
	vector<Model> planets;
	Model mercury("mercury", exePath.substr(0, exePath.find_last_of('\\')) + "\\model\\sun and planets\\mercury.obj");
	Model venus("venus", exePath.substr(0, exePath.find_last_of('\\')) + "\\model\\sun and planets\\venus.obj");
	Model earth("earth", exePath.substr(0, exePath.find_last_of('\\')) + "\\model\\sun and planets\\earthWithMoon.obj");
	Model mars("mars", exePath.substr(0, exePath.find_last_of('\\')) + "\\model\\sun and planets\\mars.obj");
	Model jupiter("jupiter", exePath.substr(0, exePath.find_last_of('\\')) + "\\model\\sun and planets\\jupiter.obj");
	Model saturn("saturn", exePath.substr(0, exePath.find_last_of('\\')) + "\\model\\sun and planets\\saturn.obj");
	Model uranus("uranus", exePath.substr(0, exePath.find_last_of('\\')) + "\\model\\sun and planets\\uranus.obj");
	Model neptune("neptune", exePath.substr(0, exePath.find_last_of('\\')) + "\\model\\sun and planets\\neptune.obj");
	planets.push_back(mercury);
	planets.push_back(venus);
	planets.push_back(earth);
	planets.push_back(mars);
	planets.push_back(jupiter);
	planets.push_back(saturn);
	planets.push_back(uranus);
	planets.push_back(neptune);
	planets.push_back(sun);
	#pragma endregion

	float ra[8] = { 0.03825,0.09488,0.10000,0.053226,1.1209,0.9449,0.4007,0.3883 };
	float radius[8] = { 0.38,0.72,1.00,1.52,5.20,9.53,19.19,30.06 };
	float velocity[8] = { 47.89,35.03,29.79,24.13,13.06,9.64,6.81,5.43 };

	glm::mat4 viewMat = glm::mat4(1.0f);
	glm::mat4 projMat = glm::mat4(1.0f);

	while (!glfwWindowShouldClose(window)) {
		processInput(window, planets);

		viewMat = camera.getViewMatrix();
		projMat = glm::perspective(glm::radians(camera.Fov), (float)800 / (float)600, 0.1f, 150.0f);

		sun.SetScale(0.5f, 0.5f, 0.5f);
		sun.SetPosition(0.0f, 0.0f, 0.0f);

		for (int i = 0; i < 8; i++) {
			planets[i].SetRotation(0.0f, glm::radians(glfwGetTime() * velocity[i] * 100), 0.0f);
			planets[i].SetScale(ra[i], ra[i], ra[i]);
			planets[i].SetPosition(radius[i] * glm::sin(glm::radians(glfwGetTime() * velocity[i]) * 0.4f), 0.0f, radius[i] * glm::cos(glm::radians(glfwGetTime() * velocity[i]) * 0.4f));
		}


		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		

		sunMat->shader->use();
		glUniformMatrix4fv(glGetUniformLocation(sunMat->shader->ID, "viewMat"), 1, GL_FALSE, glm::value_ptr(viewMat));
		glUniformMatrix4fv(glGetUniformLocation(sunMat->shader->ID, "projMat"), 1, GL_FALSE, glm::value_ptr(projMat));

		//glUniformMatrix4fv(glGetUniformLocation(objMat->shader->ID, "modelMat"), 1, GL_FALSE, glm::value_ptr(cube.GetModelMatrix()));
		//cube.DrawAcMat(objMat);
		glUniformMatrix4fv(glGetUniformLocation(sunMat->shader->ID, "modelMat"), 1, GL_FALSE, glm::value_ptr(sun.GetModelMatrix()));
		sun.Draw(sunMat);

		planetMat->shader->use();
		glUniformMatrix4fv(glGetUniformLocation(planetMat->shader->ID, "viewMat"), 1, GL_FALSE, glm::value_ptr(viewMat));
		glUniformMatrix4fv(glGetUniformLocation(planetMat->shader->ID, "projMat"), 1, GL_FALSE, glm::value_ptr(projMat));
		planetMat->SetUniform1f("material.shininess", planetMat->shininess);
		planetMat->SetUniform3f("pointLight[0].position", poiLight.position);
		planetMat->SetUniform3f("pointLight[0].color", poiLight.color);
		planetMat->SetUniform1f("pointLight[0].atten1", poiLight.attenuation1);
		planetMat->SetUniform1f("pointLight[0].atten2", poiLight.attenuation2);
		planetMat->SetUniform1f("pointLight[0].atten3", poiLight.attenuation3);
		planetMat->SetUniform3f("ambientColor", glm::vec3(0.3f, 0.3f, 0.3f));
		planetMat->SetUniform3f("viewPos", camera.Position);
		for (int i = 0; i < 8; i++) {
			glUniformMatrix4fv(glGetUniformLocation(planetMat->shader->ID, "modelMat"), 1, GL_FALSE, glm::value_ptr(planets[i].GetModelMatrix()));
			planets[i].Draw(planetMat);
		}

		// change depth function so depth test passes when values are equal to depth buffer's content
		glDepthFunc(GL_LEQUAL);
		skyboxShader->use();
		// ... 设置观察和投影矩阵 // remove translation from the view matrix
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader->ID, "viewMat"), 1, GL_FALSE, glm::value_ptr(glm::mat4(glm::mat3(viewMat))));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader->ID, "projMat"), 1, GL_FALSE, glm::value_ptr(projMat));
		skybox.DrawSkybox(skyboxMat, cubemapTexture);
		glDepthFunc(GL_LESS);


		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}