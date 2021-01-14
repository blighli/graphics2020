#include<iostream>

#include <glad/glad.h>
#include<GLFW\glfw3.h>
#include <iostream>
#include<vector>
#include<math.h>
#include<map>
#include<string>
#include<glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H
#include"shader.h"
#include"camera.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void RenderText(Shader& shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
void initText(Shader &shader);
void displayname(Shader shader);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;
struct Character {
	GLuint TextureID;   // ID handle of the glyph texture
	glm::ivec2 Size;    // Size of glyph
	glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
	GLuint Advance;    // Horizontal offset to advance to next glyph
};
std::map<GLchar, Character> Characters;
GLuint VAO, VBO;
float mposx, mposy;



Camera camera(glm::vec3(2.1,0,8));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool move_vision = false;

//相机初始化
//glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
//glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
//glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
//
//bool firstMouse = true;
//float yaw = -90.0f;
//float pitch = 0.0f;
//float lastX = 800.0f / 2.0;
//float lastY = 600.0 / 2.0;
//float fov = 45.0f;

// time
float deltaTime = 0.0f;	
float lastFrame = 0.0f;
// lighting
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
int main(int argc, char** argv[])
{
    glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(800, 800, "Project1", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	std::vector<Shader> shaders;
	Shader shader_sun("shaders/sun1.vs", "shaders/sun1.fs");
	Shader shader_earth("shaders/earth.vs", "shaders/earth.fs");
	Shader shader_moon("shaders/moon1.vs","shaders/moon1.fs");
	Shader lightCubeShader("shaders/light_cube.vs", "shaders/light_cube.fs");
	shaders.push_back(shader_sun);
	shaders.push_back(shader_earth);
	shaders.push_back(shader_moon);
	//int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	//glCompileShader(vertexShader);
	//int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	//glCompileShader(fragmentShader);
	//int shaderProgram = glCreateProgram();
	//glAttachShader(shaderProgram, vertexShader);
	//glAttachShader(shaderProgram, fragmentShader);
	//glLinkProgram(shaderProgram);
	//glDeleteShader(vertexShader);
	//glDeleteShader(fragmentShader);
	//glUseProgram(shaderProgram);
	glEnable(GL_DEPTH_TEST);

	//球的顶点，纹理与索引**************************************
	std::vector<float> points[3];
	std::vector<int>indices[3];
	float seg = 50;
	float PI = std::acos(-1);
	glm::vec3 Positions[] = {
		glm::vec3(0.0f, 0.0f, 0.0f),//sun
		glm::vec3(5.0f, 0.0f, 0.0f),//earth
		glm::vec3(4.5f, 0.0f, 0.0f)//moon
	};
	std::vector<glm::vec4 > p[3];
	for (int i = 0; i <= seg; i++)
	{
		for (int j = 0; j <= seg; j++) {
			float xse = j / seg;
			float yse = i / seg;
			float x = std::cos(xse * 2.0f * PI) * std::sin(yse * PI);
			float y = std::cos(yse * PI);
			float z = std::sin(xse * 2.0f * PI) * std::sin(yse * PI);
			p[0].push_back(glm::vec4(x, y, z, 1));
			glm::vec3 normal = glm::normalize(glm::vec3(x, y, z));
			points[0].push_back(x);
			points[0].push_back(y);
			points[0].push_back(z);
			points[0].push_back(xse * 1.0f);
			points[0].push_back(yse * 1.0f);
			points[0].push_back(normal.x);
			points[0].push_back(normal.y);
			points[0].push_back(normal.z);

			p[1].push_back(glm::vec4(x, y, z, 1));
			points[1].push_back(x / 10.0f);
			points[1].push_back(y / 10.0f);
			points[1].push_back(z / 10.0f);
			points[1].push_back(xse * 1.0f);
			points[1].push_back(yse * 1.0f);
			points[1].push_back(normal.x);
			points[1].push_back(normal.y);
			points[1].push_back(normal.z);

			p[2].push_back(glm::vec4(x, y, z, 1));
			points[2].push_back(x / 40.0f);
			points[2].push_back(y / 40.0f);
			points[2].push_back(z / 40.0f);
			points[2].push_back(xse * 1.0f);
			points[2].push_back(yse * 1.0f);
			points[2].push_back(normal.x);
			points[2].push_back(normal.y);
			points[2].push_back(normal.z);
		}
	}
	for (int i = 0; i < seg; i++)
	{
		for (int j = 0; j < seg; j++)
		{
			indices[0].push_back(i * ((int)seg + 1) + j);
			indices[0].push_back((i + 1) * ((int)seg + 1) + j);
			indices[0].push_back((i + 1) * ((int)seg + 1) + j + 1);
			indices[0].push_back(i * ((int)seg + 1) + j);
			indices[0].push_back((i + 1) * ((int)seg + 1) + j + 1);
			indices[0].push_back(i * ((int)seg + 1) + j + 1);

			indices[1].push_back(i * ((int)seg + 1) + j);
			indices[1].push_back((i + 1) * ((int)seg + 1) + j);
			indices[1].push_back((i + 1) * ((int)seg + 1) + j + 1);
			indices[1].push_back(i * ((int)seg + 1) + j);
			indices[1].push_back((i + 1) * ((int)seg + 1) + j + 1);
			indices[1].push_back(i * ((int)seg + 1) + j + 1);
		
			indices[2].push_back(i * ((int)seg + 1) + j);
			indices[2].push_back((i + 1) * ((int)seg + 1) + j);
			indices[2].push_back((i + 1) * ((int)seg + 1) + j + 1);
			indices[2].push_back(i * ((int)seg + 1) + j);
			indices[2].push_back((i + 1) * ((int)seg + 1) + j + 1);
			indices[2].push_back(i * ((int)seg + 1) + j + 1);
		}
	}
	
	//****************************************************************************
	unsigned int EBOs[3];
	unsigned int VBOs[3];
	unsigned int VAOs[3];
	glGenBuffers(3, EBOs);
	glGenVertexArrays(3, VAOs);
	glGenBuffers(3, VBOs);

	for (int i = 0; i < 3; i++) {
		glBindVertexArray(VAOs[i]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
		glBufferData(GL_ARRAY_BUFFER, points[i].size() * sizeof(float), &points[i][0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[i].size() * sizeof(int), &indices[i][0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	unsigned int texture[3];
	std::string imagepath[3];
	imagepath[0] = "image/sun.jpg";
	imagepath[1] = "image/earth.jpg";
	imagepath[2] = "image/moon.jpg";
	glGenTextures(3, &texture[0]);
	for (int i = 0; i < 3; i++) {
		glBindTexture(GL_TEXTURE_2D, texture[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		int width, height, nrChannels;
		unsigned char* data = stbi_load(imagepath[i].c_str() , &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);

	}

	//glEnable(GL_CULL_FACE);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Shader shader_text("shaders/text.vs", "shaders/text.fs");
	initText(shader_text);
	//shader.setInt("texture2", 1);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	float around_sun = 0;
	float around_earth = 0;
	float earth_self = 0;
	float moon_self = 0;
	while (!glfwWindowShouldClose(window)) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);

		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		glEnable(GL_FRONT_FACE);
		glEnable(GL_BLEND);
		/*glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);*/
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		/*RenderText(shader_text, "This is sample text", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
		RenderText(shader_text, "(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));*/
		//displayname(shader_text);

		//glCullFace(GL_FRONT);
		for (int i = 0; i < 3; i++) {
			shaders[i].use();
			shaders[i].setVec3("lightColor", 1.0f, 1.0f, 1.0f);
			shaders[i].setVec3("lightPos", lightPos);
			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			//glUniformMatrix4fv(glGetUniformLocation(shaders[i].ID, "projection"), 1, GL_FALSE, &projection[0][0]);
			glm::mat4 view = camera.GetViewMatrix();
			//glUniformMatrix4fv(glGetUniformLocation(shaders[i].ID, "view"), 1, GL_FALSE, &view[0][0]);
			shaders[i].setMat4("projection", projection);
			shaders[i].setMat4("view", view);
			glBindVertexArray(VAOs[i]);
			glBindTexture(GL_TEXTURE_2D, texture[i]);
			glm::mat4 model = glm::mat4(1.0f);
			if (i == 1) {
				float xse = around_sun / 360.f;
				float x = std::cos(xse * 2.0f * PI) * 5.0f;
				float y = 0;
				float z = std::sin(xse * 2.0f * PI) * -5.0f;;
				Positions[i] = glm::vec3(x, y, z);
				model = glm::translate(model, Positions[i]);
				float angle = earth_self;;
				model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
				//glUniformMatrix4fv(glGetUniformLocation(shaders[i].ID, "model"), 1, GL_FALSE, &model[0][0]);
				shaders[i].setMat4("model", model);
			}
			else if (i == 2) {
				float xse = around_earth / 360.f;
				float x = std::cos(xse * 2.0f * PI) * (5.0f / 393.0f + 1.0f / 4.0f) + Positions[1].x;
				float y = 0;
				float z = std::sin(xse * 2.0f * PI) * (-5.0f / 393.0f - 1.0f / 4.0f) + Positions[1].z;
				Positions[i] = glm::vec3(x, y, z);
				model = glm::translate(model, Positions[i]);
				float angle = moon_self;;
				model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
				//glUniformMatrix4fv(glGetUniformLocation(shaders[i].ID, "model"), 1, GL_FALSE, &model[0][0]);
				shaders[i].setMat4("model", model);
			}
			else {
				model = glm::translate(model, Positions[i]);
				float angle = moon_self;;
				model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
				//glUniformMatrix4fv(glGetUniformLocation(shaders[i].ID, "model"), 1, GL_FALSE, &model[0][0]);
				shaders[i].setMat4("model", model);
				glm::vec2 Mpos = glm::vec2(mposx, mposy);
				shaders[i].setVec2("Mpos", Mpos);
			}
			
			glDrawElements(GL_TRIANGLES, (int)(seg* seg) * 6, GL_UNSIGNED_INT, 0);
			
		}
		float x, y;
		x = mposx;
		y = mposy;

		around_earth = around_earth + (365.0f/27.0f)* deltaTime;
		if (around_earth >= 360.0f)
			around_earth = around_earth-360.0f;

		around_sun = around_sun + 1.0f * deltaTime;
		if (around_sun >= 360.0f)
			around_sun = around_sun-360.f;

		earth_self = earth_self + (1.0f*36.0f) * deltaTime;
		if (earth_self >= 360.0f)
			earth_self = earth_self - 360.f;

		moon_self = moon_self + (365.0f / 27.0f) * deltaTime;
		if (moon_self >= 360.0f)
			moon_self = moon_self - 360.f;
		
		

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwSwapBuffers(window);
	glfwTerminate();
    return 0;
}

//操作部分**************************************************************
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mod) {
	if (action == GLFW_PRESS) {
		if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			move_vision =true;
		}
	}
	else {
		move_vision = false;
	}
}
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	mposx = xpos;
	mposy = ypos;
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = (xpos - lastX)*50.0;
	float yoffset = (lastY - ypos)*50.0;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;
	if(move_vision==true)
		camera.ProcessMouseMovement(xoffset, yoffset);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
//***************************************************************************************

void displayname(Shader shader)
{
	
	RenderText(shader, "Earth", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
	RenderText(shader, "sun", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
	RenderText(shader, "moon", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));

}
void initText(Shader& shader)
{
	// Compile and setup the shader
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(SCR_WIDTH), 0.0f, static_cast<GLfloat>(SCR_HEIGHT));
	shader.use();
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	// FreeType
	FT_Library ft;
	// All functions return a value different than 0 whenever an error occurred
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	// Load font as face
	FT_Face face;
	if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	// Set size to load glyphs as
	FT_Set_Pixel_Sizes(face, 0, 48);

	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Load first 128 characters of ASCII set
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	// Destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);


	// Configure VAO/VBO for texture quads
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void RenderText(Shader& shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
	// Activate corresponding render state	
	shader.use();
	glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}