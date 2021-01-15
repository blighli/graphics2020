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

Camera camera(glm::vec3(0.0f, 0.2f, 4.0f), glm::radians(0.0f), glm::radians(180.0f), glm::vec3(0, 1.0f, 0));

#pragma region Light Declare
DirectionalLight dirLight = DirectionalLight(0,
	glm::vec3(glm::radians(45.0f), glm::radians(135.0f), 0.0f),
	glm::vec3(1.0f, 1.0f, 1.0f),
	0.7f,
	glm::vec3(0.0f, 0.0f, 0.0f));
PointLight pointLight[4] = { PointLight(glm::vec3(1.0f, 0.0f, 0.0f), 1.5f, glm::vec3(1.0f, 1.0f, 0.0f), 7.0f),
	PointLight(glm::vec3(0.0f, 1.0f, 1.0f), 1.5f, glm::vec3(-1.0f, 1.0f, 0.0f), 7.0f),
	PointLight(glm::vec3(1.0f, 0.0f, 1.0f), 1.5f, glm::vec3(0.0f, 1.0f, 1.0f), 7.0f),
	PointLight(glm::vec3(0.0f, 0.0f, 1.0f), 1.5f, glm::vec3(0.0f, 1.0f, -1.0f), 7.0f),
};
SpotLight spotLight = SpotLight(0, glm::vec3(glm::radians(90.0f), 0.0f, 0.0f),
	glm::vec3(1.0f, 1.0f, 0.0f),
	1.0f,
	glm::vec3(2.0f, 1.2f, 0.0f),
	glm::radians(5.0f),
	glm::radians(10.0f));
#pragma endregion

Model* now;
glm::vec3 scale = glm::vec3(0.01, 0.01, 0.01);
glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 position = glm::vec3(0.0, -0.5, 0.0);

#pragma region InPut Declare
float lastX;
float lastY;
bool firstMouse = true;
bool modelMode = false;
bool spotLightTurnOn = false;
bool pointLightOn[4] = { false,false,false,false };
bool canChange[5] = { false,false,false,false,false };

void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
	if (!modelMode) {
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
	else {
		float deltaX, deltaY;
		deltaX = xPos - lastX;
		deltaY = yPos - lastY;

		camera.LookAt(position);
		glm::vec3 CemaraToObj = position - camera.Position;
		glm::vec3 roDir = camera.Right * deltaX + camera.Up * deltaY;
		glm::vec3 roAix = glm::cross(CemaraToObj, roDir);

		glm::mat4 roMatrix(1.0f);
		roMatrix = glm::rotate(roMatrix, glm::radians(-1.0f), roAix);
		camera.Position = roMatrix * glm::vec4(-CemaraToObj, 1.0) + glm::vec4(position, 1.0);
		camera.LookAt(position);

		lastX = xPos;
		lastY = yPos;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if (!modelMode) {
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
	else {
		scale += glm::vec3(yoffset / 1000, yoffset / 1000, yoffset / 1000);
	}
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		camera.LookAt(glm::vec3(0, 0, 0));
	}
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		modelMode = false;
	}
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		modelMode = true;
	}
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE) {
		if (canChange[4]) {
			spotLightTurnOn = !spotLightTurnOn;
			canChange[4] = !canChange[4];
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
		if (!canChange[4]) {
			canChange[4] = !canChange[4];
		}
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) {
		if (canChange[0]) {
			pointLightOn[0] = !pointLightOn[0];
			canChange[0] = !canChange[0];
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		if (!canChange[0]) {
			canChange[0] = !canChange[0];
		}
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE) {
		if (canChange[1]) {
			pointLightOn[1] = !pointLightOn[1];
			canChange[1] = !canChange[1];
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		if (!canChange[1]) {
			canChange[1] = !canChange[1];
		}
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE) {
		if (canChange[2]) {
			pointLightOn[2] = !pointLightOn[2];
			canChange[2] = !canChange[2];
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
		if (!canChange[2]) {
			canChange[2] = !canChange[2];
		}
	}
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_RELEASE) {
		if (canChange[3]) {
			pointLightOn[3] = !pointLightOn[3];
			canChange[3] = !canChange[3];
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
		if (!canChange[3]) {
			canChange[3] = !canChange[3];
		}
	}
	if (!modelMode) {
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
	else {
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			position += camera.Forward * 0.01f;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			position -= camera.Forward * 0.01f;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			position -= camera.Right * 0.01f;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			position += camera.Right * 0.01f;
		}
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			position.y += 0.01f;
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			position.y -= 0.01f;
		}
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

	GLFWwindow* window = glfwCreateWindow(800, 600, "homework_3", NULL, NULL);
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

	Shader* keliShader = new Shader("keli.vert", "keli.frag");
	Shader* skyboxShader = new Shader("skybox.vert", "skybox.frag");

	Material* keliMat = new Material(keliShader, glm::vec3(0.4f, 0.4f, 0.4f), glm::vec3(0.9f, 0.9f, 0.9f), 64.0f);
	Material* skyboxMat = new Material(skyboxShader, 0, 0, 64.0f);
	

	#pragma region Model definition
	std::string exePath = argv[0];

	unsigned int cubemapTexture = LoadCubeMap(exePath.substr(0, exePath.find_last_of('\\')) + "\\skybox\\sky2");
	Mesh skybox(skyboxVertices);

	Model keli(exePath.substr(0, exePath.find_last_of('\\')) + "\\model\\keli\\keli2.0.obj");
	now = &keli;
	
	#pragma endregion

	glm::mat4 viewMat = glm::mat4(1.0f);
	glm::mat4 projMat = glm::mat4(1.0f);

	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		viewMat = camera.getViewMatrix();
		projMat = glm::perspective(glm::radians(camera.Fov), (float)800 / (float)600, 0.1f, 150.0f);

		keli.SetScale(scale.x, scale.y, scale.z);
		keli.SetRotation(rotation.x, rotation.y, rotation.z);
		keli.SetPosition(position.x, position.y, position.z);


		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		

		keliMat->shader->use();
		glUniformMatrix4fv(glGetUniformLocation(keliMat->shader->ID, "viewMat"), 1, GL_FALSE, glm::value_ptr(viewMat));
		glUniformMatrix4fv(glGetUniformLocation(keliMat->shader->ID, "projMat"), 1, GL_FALSE, glm::value_ptr(projMat));

		keliMat->SetUniform3f("ambientColor", glm::vec3(0.1f, 0.1f, 0.1f));
		keliMat->SetUniform3f("dirLight.position", dirLight.position);
		keliMat->SetUniform3f("dirLight.color", dirLight.color * dirLight.intensity);
		keliMat->SetUniform3f("dirLight.direction", dirLight.direction);
		for (int i = 0; i < 4; i++) {
			string index = to_string(i);
			string name = "pointLight[" + index + "].";
			keliMat->SetUniform3f((name + "position").c_str(), pointLight[i].position);
			if (!pointLightOn[i]) {
				keliMat->SetUniform3f((name + "color").c_str(), glm::vec3(0.0, 0.0, 0.0));
			}
			else {
				keliMat->SetUniform3f((name + "color").c_str(), (pointLight[i].color * pointLight[i].intensity));
			}
			keliMat->SetUniform1f((name + "atten1").c_str(), pointLight[i].attenuation1);
			keliMat->SetUniform1f((name + "atten2").c_str(), pointLight[i].attenuation2);
			keliMat->SetUniform1f((name + "attene").c_str(), pointLight[i].attenuation3);
		}
		//keliMat->SetUniform3f("spotLight.position", spotLight.position);
		keliMat->SetUniform3f("spotLight.position", camera.Position);
		if (!spotLightTurnOn) {
			keliMat->SetUniform3f("spotLight.color", glm::vec3(0.0, 0.0, 0.0));
		}
		else {
			keliMat->SetUniform3f("spotLight.color", spotLight.color * spotLight.intensity);
		}
		//keliMat->SetUniform3f("spotLight.direction", spotLight.direction);
		keliMat->SetUniform3f("spotLight.direction", camera.Forward);
		keliMat->SetUniform1f("spotLight.cutOff", glm::cos(spotLight.cutOff));
		keliMat->SetUniform1f("spotLight.outerCutOff", glm::cos(spotLight.outerCutOff));
		keliMat->SetUniform3f("viewPos", camera.Position);
		glActiveTexture(GL_TEXTURE10);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		keliMat->SetUniform1i("skybox", 10);

		glUniformMatrix4fv(glGetUniformLocation(keliMat->shader->ID, "modelMat"), 1, GL_FALSE, glm::value_ptr(keli.GetModelMatrix()));
		keli.Draw(keliMat);

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