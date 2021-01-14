/*
* 2021/1/13 旋转和纹理
* 2021/1/14 添加光照效果
*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <iostream>
#include <vector>
#include "Shader.h"
#include "stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void getBallPoint();
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const GLfloat PI = 3.14159265358979323846f;
const int Y_SEGMENTS = 50;
const int X_SEGMENTS = 50;

glm::vec3 cubePositions[] = {
  glm::vec3(0.0f, 0.0f, 0.0f),
  glm::vec3(4.0f, 0.0f, 0.0f),
  glm::vec3(5.0f, 0.0f, 0.0f)
};

std::vector<float> sphereVertices;
std::vector<int> sphereIndices;

//相机位置
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);                    //摄像机位置
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, -1.0f);                 //摄像机目标
glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget); //方向
//相机坐标系
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);                             //向上方向
glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));//相机正x轴
glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);          //相机正y轴
//计算速度
float deltaTime = 0.0f;
float lastFrame = 0.0f;
//鼠标位置
bool firstMouse = true;
float lastX = 400, lastY = 300;
//视角
float pitch = 0.1f;
float yaw = 0.1f;
//缩放
float Fov = 45.0f;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "PENG 22051155 earthMoonSystem", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    //初始化glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Faild to Initialize GLAD" << std::endl;
        return -1;
    }
    //绑定调整视口大小的回调
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
   // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    //#######  绑定渲染器
    Shader ourshader("src/shader_light.vs", "src/shader_light.fs");
    Shader lightshader("src/shader_3d.vs", "src/shader_3d.fs");

    //#######  数据准备
    getBallPoint();
    //创建顶点数组对象和顶点缓冲对象
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    //将数据绑定到顶点缓存区中
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), &sphereVertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(int), &sphereIndices[0], GL_STATIC_DRAW);

    //设置顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //###############  纹理准备1
    unsigned int texture1, texture2, texture3;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("img/earth.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    //############## 纹理准备2
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load("img/moon.png", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture2" << std::endl;
    }
    stbi_image_free(data);

    //############## 纹理准备3
    glGenTextures(1, &texture3);
    glBindTexture(GL_TEXTURE_2D, texture3);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load("img/sun.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture3" << std::endl;
    }
    stbi_image_free(data);

    //#####     启用深度测试
    glEnable(GL_DEPTH_TEST);

    int selfSpeed = 1;
    int earthSpeed = 4;
    int sunSpeed = 2;
    float angle = 20.0f;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);
        //########    清除缓存
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //#######    使用渲染器
        ourshader.use();
        glBindVertexArray(VAO);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        ourshader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        ourshader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        ourshader.setVec3("lightPos", cubePositions[0]);

        //###############  变换
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        //LookAt矩阵
        view = glm::lookAt(
            cameraPos,           //位置
            cameraTarget,        //目标
            cameraUp);           //上向量
        ourshader.setMat4("view", view);

        //投影矩阵
        projection = glm::perspective(glm::radians(Fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        unsigned int projectionLoc = glGetUniformLocation(ourshader.ID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        //

        //模型矩阵
        for (unsigned int i = 0; i < 3; i++) {
            model = glm::mat4(1.0f);
            //太阳
            if (i == 0) {
                lightshader.use();
                glBindVertexArray(VAO);
                lightshader.setMat4("view", view);
                lightshader.setMat4("projection", projection);

                model = glm::rotate(model, glm::radians(angle) + currentFrame * selfSpeed, glm::vec3(0.0f, 1.0f, 0.0f));//自转
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture3); 
                lightshader.setMat4("model", model);
            }

            //地球
            if (i == 1) {
                ourshader.use();
                glBindVertexArray(VAO);
                model = glm::rotate(model, glm::radians(angle) + currentFrame * sunSpeed, glm::vec3(0.0f, 1.0f, 0.0f));//绕太阳公转
                model = glm::translate(model, cubePositions[i]);                                                        //定位
                model = glm::rotate(model, glm::radians(angle) + currentFrame * selfSpeed, glm::vec3(0.0f, 1.0f, 0.0f));//自转
                model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));                                                 //缩放

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture1);

                ourshader.setMat4("model", model);
            }
            //月亮
            if (i == 2) {
                ourshader.use();
                glBindVertexArray(VAO);


                model = glm::translate(model, cubePositions[0]);
                model = glm::rotate(model, glm::radians(angle) + currentFrame * sunSpeed, glm::vec3(0.0f, 1.0f, 0.0f)); //绕太阳公转
                model = glm::translate(model, -cubePositions[0]);

                model = glm::translate(model, cubePositions[1]);                                                         //公转坐标-地球
                model = glm::rotate(model, glm::radians(angle) + currentFrame * earthSpeed, glm::vec3(0.0f, 1.0f, 0.0f));//绕地球公转
                model = glm::translate(model, -cubePositions[1]);

                model = glm::translate(model, cubePositions[i]);                                                        //定位


                model = glm::rotate(model, glm::radians(angle) + currentFrame * selfSpeed, glm::vec3(0.0f, 1.0f, 0.0f));//自转
                model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));                                                 //缩放

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture2);
                ourshader.setMat4("model", model);
            }
            glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraTarget;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraTarget;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraTarget, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraTarget, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = 89.0f;
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraTarget = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (Fov >= 1.0f && Fov <= 45.0f)
        Fov -= yoffset;
    if (Fov <= 1.0f)
        Fov = 1.0f;
    if (Fov >= 45.0f)
        Fov = 45.0f;
}

void getBallPoint() {
    //生成球的顶点
    int flag = 0;
    for (int y = 0; y <= Y_SEGMENTS; y++)
    {
        for (int x = 0; x <= X_SEGMENTS; x++)
        {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;
            float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = std::cos(ySegment * PI);
            float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            sphereVertices.push_back(xPos);
            sphereVertices.push_back(yPos);
            sphereVertices.push_back(zPos);
            sphereVertices.push_back((float)x / (float)X_SEGMENTS);
            sphereVertices.push_back(1 - ((float)y / (float)Y_SEGMENTS));

        }
    }
    //生成球的Indices
    for (int i = 0; i < Y_SEGMENTS; i++)
    {
        for (int j = 0; j < X_SEGMENTS; j++)
        {
            sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
            sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j);
            sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);
            sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
            sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);
            sphereIndices.push_back(i * (X_SEGMENTS + 1) + j + 1);
        }
    }
}

