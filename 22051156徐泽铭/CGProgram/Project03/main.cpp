//
// Created by Mirocos on 2021/1/14.
//

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#define STB_IMAGE_IMPLEMENTATION

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <shader.h>
#include <camera.h>
#include <model.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);


const unsigned  int SCR_WIDTH = 800;
const unsigned  int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f) );
//Camera camera(glm::vec3(-0.868463, 0.808117, -1.2753));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lightning
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec3 modelPos = glm::vec3(-0.1f, -0.4f, 0.0f);
glm::mat4 model = glm::mat4(1.0f);

bool isModelTransform = false;

const float MODELMOVESPEED = 100.f;
const float MODELROTATESPEED = 2.f;
const float MODELSCALESPEED = 1.01f;

using Model_Movement = Camera_Movement;
void ModelProcessKeyboard(Model_Movement direction, float deltaTime);
void ModelProcessRotateXY(float xoffset, float yoffset);
void ModelProcessRotateZ(float offset);
void ModelProcessZoom(enum ZOOM type, float deltaTime);

int main(){

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // object


    // lighting

    Shader  modelShader("modellight.vs", "modellight.fs");

//    Shader modelShader("../ShaderResource/1.colors.vs", "../ShaderResource/1.colors.fs");
//    Shader lightCubeShader("../ShaderResource/1.light_cube.vs", "../ShaderResource/1.light_cube.fs");

    Model ourModel("kuuga/KMJ.obj");

    glm::vec3 pointLightPositions[] = {
            glm::vec3( 0.7f,  0.2f,  2.0f),
            glm::vec3( 2.3f, -3.3f, -4.0f),
            glm::vec3(-4.0f,  2.0f, -12.0f),
            glm::vec3( 0.0f,  0.0f, -3.0f)
    };

    // render the loaded model
    model = glm::translate(model, modelPos); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));	// it's a bit too big for our scene, so scale it down
//    model = glm::rotate(model, glm::radians(90.f), glm::vec3(1.0f ,0.0f ,0.0f));



    while(!glfwWindowShouldClose(window)){
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        // render
        // ----------------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 400.f);
        glm::mat4 view = camera.GetViewMatrix();








        modelShader.use();

        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);

//        // render the loaded model
//        glm::mat4 model = glm::mat4(1.0f);
//        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
//        model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));	// it's a bit too big for our scene, so scale it down
        modelShader.setMat4("model", model);


        modelShader.setVec3("viewPos", camera.Position);

        for(int i = 0; i < 4; i++){
            modelShader.setFloat("pointLights["+ std::to_string(i) + "].constant", 1.0f);
            modelShader.setFloat("pointLights["+ std::to_string(i) + "].linear", 0.09f);
            modelShader.setFloat("pointLights["+ std::to_string(i) + "].quadratic", 0.032f);
            modelShader.setVec3("pointLights["+ std::to_string(i) + "].position", pointLightPositions[i]);
            modelShader.setVec3("pointLights["+ std::to_string(i) + "].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
            modelShader.setVec3("pointLights["+ std::to_string(i) + "].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
            modelShader.setVec3("pointLights["+ std::to_string(i) + "].specular",glm::vec3(1.0f, 1.0f, 1.0f));
        }

        modelShader.setVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
        modelShader.setVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        modelShader.setVec3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
        modelShader.setVec3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));


        modelShader.setVec3("spotLight.position", camera.Position);
        modelShader.setVec3("spotLight.direction", camera.Front);
        modelShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        modelShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        modelShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        modelShader.setFloat("spotLight.constant", 1.0f);
        modelShader.setFloat("spotLight.linear", 0.09);
        modelShader.setFloat("spotLight.quadratic", 0.032);
        modelShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        modelShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));



        ourModel.Draw(modelShader);
        glfwSwapBuffers(window);
        glfwPollEvents();

    }


    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;



}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        isModelTransform = true;
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        isModelTransform = false;



    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        if(!isModelTransform)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        else
            ModelProcessKeyboard(FORWARD, deltaTime);

    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        if(!isModelTransform)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        else
            ModelProcessKeyboard(BACKWARD, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        if(!isModelTransform)
            camera.ProcessKeyboard(LEFT, deltaTime);
        else
            ModelProcessKeyboard(LEFT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        if(!isModelTransform)
            camera.ProcessKeyboard(RIGHT, deltaTime);
        else
            ModelProcessKeyboard(RIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
        if(!isModelTransform)
            camera.ProcessKeyboard(TOP, deltaTime);
        else
            ModelProcessKeyboard(TOP, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){

        if(!isModelTransform)
            camera.ProcessKeyboard(DOWN, deltaTime);
        else
            ModelProcessKeyboard(DOWN, deltaTime);
    }


    if(glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS && isModelTransform){
        ModelProcessZoom(ZOOM_IN, deltaTime);
    }

    if(glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && isModelTransform){
        ModelProcessZoom(ZOOM_OUT, deltaTime);
    }



//    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
//    std:: cout<< "mouse dected..." << std::endl;
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

    if(!isModelTransform)
        camera.ProcessMouseMovement(xoffset, yoffset);
    else
        ModelProcessRotateXY(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if(!isModelTransform)
        camera.ProcessMouseScroll(yoffset);
    else
        ModelProcessRotateZ(yoffset);
}


void ModelProcessKeyboard(Model_Movement direction, float deltaTime)
{
    float velocity = MODELMOVESPEED * deltaTime;
    std::cout << "velocity: " << velocity << std::endl;
    if (direction == FORWARD)
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f) * velocity);
    if (direction == BACKWARD)
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.0f) * velocity);
    if (direction == LEFT)
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 0.0f) * velocity);
    if (direction == RIGHT)
        model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f) * velocity);
    if (direction == TOP)
        model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f) * velocity);
    if (direction == DOWN)
        model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f) * velocity);
}

void ModelProcessRotateXY(float xoffset, float yoffset){
    float rotate_angle_y_axis = MODELROTATESPEED * xoffset;
    float rotate_angle_x_axis = MODELROTATESPEED * yoffset;
//    model = glm::translate(model, modelPos);
    model = glm::rotate(model, glm::radians(rotate_angle_y_axis), glm::vec3(0.f, 1.f, 0.f));
    model = glm::rotate(model, glm::radians(rotate_angle_x_axis), glm::vec3(1.f, 0.f, 0.f));
//    model = glm::translate(model, -modelPos);

}


void ModelProcessRotateZ(float offset){
    float rotate_angle_z_axis = MODELROTATESPEED * offset * 2.f;
//    model = glm::translate(model, modelPos);
    model = glm::rotate(model, glm::radians(rotate_angle_z_axis), glm::vec3(0.f, 0.f, 1.f));
//    model = glm::translate(model, -modelPos);
}


void ModelProcessZoom(enum ZOOM type, float deltaTime){
    glm::vec3 SCALE = glm::vec3(1.f);
    if(type == ZOOM_IN)
        SCALE *= MODELSCALESPEED;
    else
        SCALE /= MODELSCALESPEED;

    model = glm::scale(model, SCALE);
}