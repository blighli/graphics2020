#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "framework/Shader.h"
#include "framework/Camera.h"
#include "framework/Model.h"
#include "main.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void getCube();
void addCubeLight(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3 ambient = glm::vec3(1.0f, 1.0f, 1.0f),glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f),
                 glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f),float constant = 1.0f,float linear = 0.09f,float quadratic = 0.032, glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f));
    
// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//��Դλ��
glm::vec3 cubePositions[] = {
    glm::vec3(1.3f,  4.2f,  1.5f),
    glm::vec3(-1.3f, 4.3f,  -1.5f)
};

std::vector<struct cubeLight> cubeLight;
struct Cube aCube;

int main()
{
    // ��ʼ������
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "PENG 22051155 project03 3DModel", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    //�󶨻ص�
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    //����ʾ��ͷ
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    // glad��ʼ��
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    //��ͼ��ֱ��ת
    //stbi_set_flip_vertically_on_load(true);

    // ������Ȳ���
    glEnable(GL_DEPTH_TEST);

    // ����Ⱦ��
    Shader ourShader("src/model_shader.vs", "src/model_shader.fs");
    Shader lightCubeShader("src/shader_3d.vs", "src/shader_3d.fs");

    // ����ģ��
    Model ourModel(("img/nanosuit/nanosuit.obj"));
    getCube();

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    addCubeLight( glm::vec3(1.3f, 4.2f, 1.5f),
                    glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, 0.4f, 0.4f),
                    glm::vec3(0.0f, 1.0f, 1.0f),
                    1.0f, 0.09f, 0.032, glm::vec3(0.0f, 1.0f, 1.0f));

    addCubeLight( glm::vec3(-1.3f, 4.3f, -1.5f),
                    glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(0.4f, 0.0f, 0.0f),
                    glm::vec3(1.0f, 0.0f, 0.0f),
                    1.0f, 0.09f, 0.032, glm::vec3(1.0f, 0.0f, 0.0f));
    
    // ��Ⱦ
    while (!glfwWindowShouldClose(window))
    {
        // ֡��ʱ
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // �����¼�
        processInput(window);

        // ��Ⱦ�������ɫ�������Ȼ���
        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();

        // ��ͼ�����ͶӰ����
        // �����lookAt���������Ϊ������е�����
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // ģ�;���
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        

        ourShader.use();
        ourShader.setVec3("viewPos", camera.Position);
        ourShader.setInt("pointLightNum", 2);
        //// directional light
        ourShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        ourShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        ourShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);


        //// point light 1
        ourShader.setVec3("pointLights[0].position", cubeLight[0].position);
        ourShader.setVec3("pointLights[0].ambient", cubeLight[0].ambient);
        ourShader.setVec3("pointLights[0].diffuse", cubeLight[0].diffuse);
        ourShader.setVec3("pointLights[0].specular", cubeLight[0].specular);
        ourShader.setFloat("pointLights[0].constant", cubeLight[0].constant);
        ourShader.setFloat("pointLights[0].linear", cubeLight[0].linear);
        ourShader.setFloat("pointLights[0].quadratic", cubeLight[0].quadratic);

        //// point light 2
        ourShader.setVec3("pointLights[1].position", cubeLight[1].position);
        ourShader.setVec3("pointLights[1].ambient", cubeLight[1].ambient);
        ourShader.setVec3("pointLights[1].diffuse", cubeLight[1].diffuse);
        ourShader.setVec3("pointLights[1].specular", cubeLight[1].specular);
        ourShader.setFloat("pointLights[1].constant", cubeLight[1].constant);
        ourShader.setFloat("pointLights[1].linear", cubeLight[1].linear);
        ourShader.setFloat("pointLights[1].quadratic", cubeLight[1].quadratic);

        //// spotLight
        ourShader.setVec3("spotLight.position", camera.Position);
        ourShader.setVec3("spotLight.direction", camera.Front);
        ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        ourShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("spotLight.constant", 1.0f);
        ourShader.setFloat("spotLight.linear", 0.09);
        ourShader.setFloat("spotLight.quadratic", 0.032);
        ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        ourModel.Draw(ourShader);
         
        //���ƹ�Դ
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        
        glBindVertexArray(aCube.VAO);
        for (unsigned int i = 0; i < 2; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            model = glm::scale(model, glm::vec3(0.1f)); // a smaller cube
            lightCubeShader.setMat4("model", model);
            lightCubeShader.setVec3("lightColor", cubeLight[i].color);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glfwSwapBuffers(window);
        glfwPollEvents(); 
    }
    
    glDeleteVertexArrays(1, &aCube.VAO);
    glDeleteBuffers(1, &aCube.VBO);

    glfwTerminate();
    return 0;
}

// ���̰����󶨵���Ӧ����
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

// ���ڴ�С�任ʱ���õĺ���
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// ����ƶ�����Ӧ����
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// �����ְ󶨵���Ӧ����
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void getCube() {
    //���������������Ͷ��㻺�����

    glGenVertexArrays(1, &aCube.VAO);
    glGenBuffers(1, &aCube.VBO);
    glGenBuffers(1, &aCube.EBO);

    //�����ݰ󶨵����㻺������
    glBindVertexArray(aCube.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, aCube.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, aCube.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //���ö�������ָ��
    //����location 0 ,3��ֵ�����ͣ���׼���񣬴�С��ƫ����
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    //############## ����׼��
    glGenTextures(1, &aCube.texture1);
    glBindTexture(GL_TEXTURE_2D, aCube.texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    //stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("img/container.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture1" << std::endl;
    }
    stbi_image_free(data);
}

void addCubeLight(glm::vec3 position ,glm::vec3 ambient ,glm::vec3 diffuse,
    glm::vec3 specular ,float constant ,float linear ,float quadratic,glm::vec3 color) {
    cubeLight.push_back(struct cubeLight(position, ambient, diffuse, specular, constant, linear, quadratic,color));
}