// Local Headers
#include "glitter.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <random>
#include <string>

#include "Shader.h"
#include "Camera.h"
#include "Light.h"
#include "Texture.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = mWidth;
const unsigned int SCR_HEIGHT = mHeight;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
float texMix = 0.2f;

void RenderScene(Shader);
void RenderLights(Shader);

// TODO: move buffers to their own classes at some point
unsigned int VAO;
Texture tex1, tex2;
std::vector<Light> lights;
std::vector<glm::vec3> boxPositions;

int main(int argc, char * argv[]) 
{
    // Load GLFW and Create a Window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    auto mWindow = glfwCreateWindow(mWidth, mHeight, "OpenGL", nullptr, nullptr);

    // Check for Valid Context
    if (mWindow == nullptr) {
        fprintf(stderr, "Failed to Create OpenGL Context");
        return EXIT_FAILURE;
    }

    // Create Context and Load OpenGL Functions
    glfwMakeContextCurrent(mWindow);
    glfwSetFramebufferSizeCallback(mWindow, framebuffer_size_callback);
    glfwSetCursorPosCallback(mWindow, mouse_callback);
    glfwSetScrollCallback(mWindow, scroll_callback);

    gladLoadGL();
    fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

    // Vertex info for a cube
    float vertices[] = {
        // Position           // UV         // Normals
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,   0, 0, -1,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,   0, 0, -1,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   0, 0, -1,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   0, 0, -1,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   0, 0, -1,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,   0, 0, -1,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   0, 0, 1,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   0, 0, 1,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,   0, 0, 1,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,   0, 0, 1,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,   0, 0, 1,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   0, 0, 1,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1, 0, 0,   
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  -1, 0, 0,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1, 0, 0,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1, 0, 0,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  -1, 0, 0,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1, 0, 0,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   1, 0, 0,   
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   1, 0, 0,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   1, 0, 0,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   1, 0, 0,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   1, 0, 0,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   1, 0, 0,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   0, -1, 0,   
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,   0, -1, 0,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   0, -1, 0,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   0, -1, 0,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   0, -1, 0,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   0, -1, 0,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   0, 1, 0,   
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   0, 1, 0,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   0, 1, 0,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   0, 1, 0,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,   0, 1, 0,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   0, 1, 0
    };

    // use our shader program when we want to render an object
    Shader lightShader("../Glitter/Shaders/light.vert", "../Glitter/Shaders/light.frag");
    Shader boxShader("../Glitter/Shaders/box.vert", "../Glitter/Shaders/box.frag");

    // ===================================================================
    // Setup for textures
    //
    tex1 = Texture("Textures/revue.jpg");
    tex2 = Texture("Textures/awesomeface.png");
    // Pass in texture info

    // TODO: How is tex info being passed into the uniform withouth this?
    //glUniform1i(glGetUniformLocation(boxShader.ID, "tex"), 0);

    // ===================================================================
    // generating a VAO
    glGenVertexArrays(1, &VAO);

    // Initialization code, which is typically done once 
    // unless your object will freq change
    glBindVertexArray(VAO);

    // buffer objects allow us to send large batches of data at once to the GPU
    // so that we don't have to send data vertex by vertex
    unsigned int VBO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // copies vertex data in buffer's memory
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // parameter descriptions:
    // 1. Which vertex attrib we want to configure. Relates to the layout location 
    // 2. Size of the vertex attribute so vec3 is 3 values. 
    // 3. The type of data
    // 4. Do we want data to be normalized? 
    // 5. Stride of data: the space between consecutive vertex attribs
    // 6. Offset of the attrib data. Needs to be casted to void*    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);   // position
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // texture
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float))); // normals
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    // unbinding VBO
    // this is allowed, since the call to glVertexAttribPointer registered VBO
    // as the vertex attribute's bound vertex buffer object so afterwards we 
    // can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // unbinding VAO for later use
    glBindVertexArray(0);

    // ===================================================================
    // Light information
    // 
    std::vector<glm::vec3> lightPositions = {
        glm::vec3( 1.0f,  0.0f, -2.0f),
        glm::vec3( 1.0f,  2.0f, -3.0f),
        glm::vec3(-3.5f,  2.5f, -4.0f),
        glm::vec3(-2.4f, -0.5f, -5.0f),
        glm::vec3( 1.5f, -1.0f, -7.0f),
    };

    std::vector<glm::vec3> lightColors = {
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 1.0f),
    };

    for (int i = 0; i < lightPositions.size(); ++i)
    {
        lights.push_back(Light(lightPositions[i], lightColors[i]));   
    }

    boxPositions = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 0.1f,  0.0f, -1.5f),
        //     glm::vec3(-3.5f,  2.5f, -4.0f),
        //     glm::vec3(-2.4f, -0.5f, -5.0f),
        //     glm::vec3( 1.5f, -1.0f, -7.0f),
    };

    // ===================================================================
    // Bind UBO block index to shaders
    glUniformBlockBinding(boxShader.ID   , glGetUniformBlockIndex(boxShader.ID, "Matrices"), 0);
    glUniformBlockBinding(lightShader.ID , glGetUniformBlockIndex(lightShader.ID, "Matrices"), 0);

    glUniformBlockBinding(boxShader.ID   , glGetUniformBlockIndex(lightShader.ID, "LightBlock"), 1);

    // Create a uniform buffer to handle viewprojection and lights
    unsigned int uboMatrices;
    glGenBuffers(1, &uboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

    unsigned int uboLights;
    glGenBuffers(1, &uboLights);
    glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
    glBufferData(GL_UNIFORM_BUFFER, lightPositions.size() * sizeof(Light), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, 1, uboLights, 0, lightPositions.size() * sizeof(Light));

    // ====================================================
    // Frame buffer for depth map
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);

    // Create a 2D texture to store framebuffer's depth buffer
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
            SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glEnable(GL_DEPTH_TEST);

    // ===================================================================
    // Rendering Loop
    while (glfwWindowShouldClose(mWindow) == false) 
    {
        // per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        // TODO add an FPS counter
        // have it render to the screen
        // look at text rendering tutorials
        //printf("%f ms/frame\n", 1000.0/double
        lastFrame = currentFrame;

        processInput(mWindow); 

        glm::mat4 view  = glm::mat4(1.0f);
        view = camera.GetViewMatrix();

        glm::mat4 proj  = glm::mat4(1.0f);
        proj = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f);

        // Send the view and projection matrices to the UBO
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(proj));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // Background Fill Color
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            RenderScene(boxShader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        RenderScene(boxShader);

        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }   

    glfwTerminate();
    return EXIT_SUCCESS;
}

void RenderLights(Shader lightShader)
{
    // Draw our lights
    lightShader.use();
    for (int i = 0; i < lights.size(); ++i)
    {
        glm::vec3 newPos = lights[i].pos;
        // first light is moving light
        if (i == 0)
        {
            float radius = 5.0f;
            float omega = 1.0f;

            newPos += radius * glm::vec3(cos(omega * glfwGetTime()), 
                    0.0f,
                    sin(omega * glfwGetTime()));
        }

        glm::mat4 model = glm::mat4(1.0f); 
        model = glm::translate(model, newPos);
        model = glm::scale(model, glm::vec3(0.5f));

        /*
         * TODO make this with UBOs
        // Also add our light info to each box
        boxShader.use();
        // Send light info to UBO
        // TODO: figure out how to pass in struct info to UBOs
        glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec3), glm::value_ptr(newPos));
        glBufferSubData(GL_UNIFORM_BUFFER, 16, sizeof(glm::vec3), glm::value_ptr(lights[i].color));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        std::string lightPos = "lights[" + std::to_string(i) + "].pos";
        std::string lightColor = "lights[" + std::to_string(i) + "].color";
        glUniform3fv(glGetUniformLocation(boxShader.ID, lightPos.c_str()), 1, glm::value_ptr(newPos));
        glUniform3fv(glGetUniformLocation(boxShader.ID, lightColor.c_str()), 1, glm::value_ptr(lights[i].color));
        */

        lightShader.use();
        // send in model
        glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

        // Pass in light color to shader
        glUniform3fv(glGetUniformLocation(lightShader.ID, "lightColor"), 1, glm::value_ptr(lights[i].color));

        // Draw the object
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }

}

void RenderScene(Shader sceneShader)
{
    // Add texture to our box
    sceneShader.use();
    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex1.ID);
    //glActiveTexture(GL_TEXTURE1);
    //glBindTexture(GL_TEXTURE_2D, tex2);

    glm::mat4 model = glm::mat4(1.0f);

    // Draw the four test boxes for UBO testing
    glm::vec3 scale = glm::vec3(2.0f);

    // Draw our box
    sceneShader.use();
    for (int i = 0; i < boxPositions.size(); ++i)
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, boxPositions[i]);
        glUniformMatrix4fv(glGetUniformLocation(sceneShader.ID, "model"),
                1, GL_FALSE, glm::value_ptr(model));

        // Draw the box
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
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

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        camera.MovementSpeed = maxSpeed;
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
    {
        camera.MovementSpeed = normalSpeed;
    }
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
