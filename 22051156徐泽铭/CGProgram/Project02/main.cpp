#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "sphere.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>
#include <camera.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 800;



Camera camera(glm::vec3(0.f, 0.f, 3.f));

glm::vec3 sunPos = glm::vec3(0.0f, 0.0f, -100.f);
glm::vec3 earthPos = glm::vec3(80.0f, 0.0f, -110.0f);
glm::vec3 moonPos = glm::vec3(100.0f, 0.0f, -120.0f);


glm::vec3 curSunPos = sunPos;
glm::vec3 curEarthPos = earthPos;
glm::vec3 curMoonPos = moonPos;


// rotate axis
glm::vec3 earth_sun_axis = glm::vec3(0.0, 21.0, 0.0f);


glm::mat4 projection;
glm::mat4 view;
glm::mat4 inverse_sun_model;
glm::mat4 inverse_earth_model;
glm::mat4 inverse_moon_model;

bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
float deltaTime = 0.f;
float lastFrame = 0.f;

float rotateDegree = 0.f;
float earth_sun_degree = 0.f;
float earth_self_degree = 0.f;
float moon_earth_degree = 0.f;
float moon_self_degree = 0.f;


// sphere size
float sun_ratio = 30.f;
float earth_ratio = 10.f;
float moon_ratio = 5.f;

glm::vec3 lightPos = sunPos;


std::ostream& operator<<(std::ostream& os, glm::vec3& v){
    os << v.x << " " << v.y << " " << v.z;
    return os;

}

std::ostream& operator<<(std::ostream& os, glm::vec2& v){
    os << v.x << " " << v.y;
    return os;
}


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "SolarSystem", NULL, NULL);
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

    // capture mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }











    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------

    Sphere sun(sun_ratio, 64, 32);
    sun.setShader("sun.vs", "sun.fs");
    sun.setTexture("2k_sun.jpg");
    sun.init();

    Sphere earth(earth_ratio, 64, 32);
    earth.setShader("solarsystem.vs", "solarsystem.fs");
    earth.setTexture("Albedo-diffuse.jpg");
    earth.init();


    Sphere moon(moon_ratio, 64, 32);
    moon.setShader("solarsystem.vs", "solarsystem.fs");
    moon.setTexture("moon1024.bmp");
    moon.init();


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {

        rotateDegree  += 10.0f;
        earth_sun_degree += 30.f / 365.f;
        earth_self_degree += 10.f;
        moon_self_degree += 10.f;
        moon_earth_degree += 120.f / 365.f;
        // update time per-frame
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 0.8f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        projection    = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / SCR_HEIGHT, 0.1f, 600.f);
        view = camera.GetViewMatrix();

        // draw sun
        sun.use();
        glm::mat4 sun_model         = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        sun_model = glm::translate(sun_model, sunPos);
        sun_model = glm::rotate(sun_model, glm::radians(90.0f), glm::vec3(1.f, 0.f, 0.f));

        inverse_sun_model = glm::inverse(sun_model);

        glm::vec4 tempSunPos = glm::vec4(sunPos, 1.0f);
        tempSunPos = view * sun_model * tempSunPos;
        tempSunPos /= tempSunPos.w;
        curSunPos = glm::vec3(tempSunPos.x, tempSunPos.y, tempSunPos.z);
//        std::cout << "curSunPos :" << curSunPos << std::endl;
//
        sun.shader.setMat4("projection", projection);
        sun.shader.setMat4("view", view);
        sun.shader.setMat4("model", sun_model);
        sun.shader.setFloat("specularStrength", 0.5);
        sun.shader.setFloat("ambientStrength", 0.1);
        sun.shader.setVec3("lightPos", lightPos);
        sun.shader.setVec3("viewPos", camera.Position);
        sun.shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        sun.shader.setInt("texture1", 2);
        glActiveTexture(GL_TEXTURE2);
        sun.draw();


        // draw earth
        earth.use();
        // create transformations
        glm::mat4 earth_model         = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        earth_model = glm::translate(earth_model, sunPos);
        earth_model = glm::rotate(earth_model, glm::radians(-earth_sun_degree), earth_sun_axis);
        earth_model = glm::translate(earth_model, -sunPos);
        earth_model = glm::translate(earth_model, earthPos);
        earth_model = glm::rotate(earth_model, glm::radians(-earth_self_degree), glm::vec3(-1.f, 10.f, 0.f));
        earth_model = glm::rotate(earth_model, glm::radians(-90.0f), glm::vec3(1.f, 0.f, 0.f));

        glm::vec4 tempEarthPos = glm::vec4(earthPos, 1.0f);
        tempEarthPos = earth_model * tempEarthPos;
        tempEarthPos /= tempEarthPos.w;
        curEarthPos = glm::vec3(tempEarthPos.x, tempEarthPos.y, tempEarthPos.z);


        // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
        earth.shader.setMat4("projection", projection);
        earth.shader.setMat4("view", view);
        earth.shader.setMat4("model", earth_model);
        earth.shader.setFloat("specularStrength", 0.5);
        earth.shader.setFloat("ambientStrength", 0.1);
        earth.shader.setVec3("lightPos", lightPos);
        earth.shader.setVec3("viewPos", camera.Position);
        earth.shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        earth.shader.setInt("texture1", 0);
        glActiveTexture(GL_TEXTURE0);
        earth.draw();


        // draw moon
        moon.use();
        glm::mat4 moon_model         = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        moon_model = glm::translate(moon_model, sunPos);
        moon_model = glm::rotate(moon_model, glm::radians(-earth_sun_degree), earth_sun_axis);
        moon_model = glm::translate(moon_model, -sunPos);
        moon_model = glm::translate(moon_model, earthPos);
        moon_model = glm::rotate(moon_model, glm::radians(-moon_earth_degree), glm::vec3(-2.0, 9.0, 0.0f));
        moon_model = glm::translate(moon_model, -earthPos);
        moon_model = glm::translate(moon_model, moonPos);
        moon_model = glm::rotate(moon_model, glm::radians(-moon_self_degree), glm::vec3(-1.f, 10.f, 0.f));


        glm::vec4 tempMoonPos = glm::vec4(moonPos, 1.0f);
        tempMoonPos = projection * view * moon_model * tempMoonPos;
        tempMoonPos /= tempMoonPos.w;
        curMoonPos = glm::vec3(tempMoonPos.x, tempMoonPos.y, tempMoonPos.z);


        moon.shader.setMat4("projection", projection);
        moon.shader.setMat4("view", view);
        moon.shader.setMat4("model", moon_model);
        moon.shader.setFloat("specularStrength", 0.5);
        moon.shader.setFloat("ambientStrength", 0.1);
        moon.shader.setVec3("lightPos", lightPos);
        moon.shader.setVec3("viewPos", camera.Position);
        moon.shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        moon.shader.setInt("texture1", 1);
        glActiveTexture(GL_TEXTURE1);
        moon.draw();


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

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

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}


