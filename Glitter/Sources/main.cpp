// Local Headers
#include "glitter.hpp"

#include "TentGui.h"

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
#include <chrono>

// My headers
#include "Shader.h"
#include "ShaderController.h"
#include "Camera.h"
#include "Light.h"
#include "Texture.h"
#include "ObjectManager.h"
#include "LightManager.h"
#include "FrameBuffer.h"
#include "Cube.h"
#include "Quad.h"
#include "Game.h"
#include "Cubemap.h"
#include "SceneLoader.h"
#include "Model.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

const int TAG_LENGTH = 32;

// cameras
Camera camera(glm::vec3(0.0f, 1.0f, 7.0f));
Camera gameCamera(glm::vec3(0.0f, 0.0f, 5.0f));

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;

// TODO: move buffers to their own classes at some point
GLuint VAO;
GLuint uboLights;

ObjectManager objectManager;
// TODO move to resource manager
std::vector<FrameBuffer> renderPasses;

// TODO move to resource manager
ShaderController shaderController;
// TODO move to resource manager
// TextureMaster

TentGui tentGui;

GameController GAME;

int main(int argc, char * argv[])
{
    // Load GLFW and Create a Window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    auto mWindow = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", nullptr, nullptr);

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

    // Initialize imgui context
    tentGui.Init(mWindow);
    tentGui.activeCamera = &camera;

    // use our shader program when we want to render an object
    Shader genericShader("../Glitter/Shaders/generic.vert", "../Glitter/Shaders/generic.frag");
    Shader lightShader("../Glitter/Shaders/light.vert", "../Glitter/Shaders/light.frag");
    Shader screenShader("../Glitter/Shaders/postProcess.vert", "../Glitter/Shaders/kernel.frag");
    // Add shader to shaderController for hot reloading
    // TODO handle this seamlessly so that theres no need to add shader each time to controller
    shaderController.Add("generic", &genericShader);
    shaderController.Add("light", &lightShader);
    shaderController.Add("screen", &screenShader);

    SceneLoader loader;
    //Testing
    std::string path = R"(./Scenes/test.json)";
    //loader.LoadScene(path.c_str());

    // ===================================================================
    // Setup for textures
    //
    Texture tex1("Textures/wall.jpg");
    Texture tex2("Textures/uv.png");

    std::vector<std::string> faces =
    {
        "Textures/skybox/right.jpg",
        "Textures/skybox/left.jpg",
        "Textures/skybox/up.jpg",
        "Textures/skybox/down.jpg",
        "Textures/skybox/front.jpg",
        "Textures/skybox/back.jpg"
    };

    Cubemap skyboxTexture(faces);
    Shader skyboxShader("../Glitter/Shaders/skybox.vert", "../Glitter/Shaders/skybox.frag");
    Cube skybox;
    skybox.shader = &skyboxShader;
    // ===================================================================

    Quad screenQuad;
    screenQuad.shader = &screenShader;

    // TODO replace this into some more manageable format, like glTF
    // ===================================================================
    // Light information
    //
    std::vector<glm::vec3> lightPositions = {
        glm::vec3( 1.0f,  0.0f,  0.0f),
        glm::vec3(-5.0f,  2.0f,  0.0f),
        glm::vec3( 5.0f, -3.0f, -1.0f),
        glm::vec3( 0.0f, -2.0f,  1.0f),
        glm::vec3( 3.0f,  1.0f,  2.0f),
    };

    std::vector<std::string> lightNames = {
        "Red light",
        "Green light",
        "Blue light",
        "Purple light",
        "Cyan light"
    };

    std::vector<glm::vec3> lightColors = {
        glm::vec3(1.0f, 0.0f, 0.0f), // red
        glm::vec3(0.0f, 1.0f, 0.0f), // green
        glm::vec3(0.0f, 0.0f, 1.0f), // blue
        glm::vec3(1.0f, 0.0f, 1.0f), // purple
        glm::vec3(0.0f, 1.0f, 1.0f), // cyan
    };

    // TODO move model info into GlObjects
    std::vector<glm::vec3> boxPositions = {
        glm::vec3( 2.0f,  0.0f,  0.0f),
        glm::vec3(-5.0f,  3.0f,  0.0f),
        glm::vec3( 5.0f, -3.0f, -2.0f),
        glm::vec3( 1.0f, -2.0f,  1.0f),
        glm::vec3( 3.0f,  2.0f,  2.0f),
    };

    // ===================================================================
    // Bind UBO block index to shaders
    // TODO handle this by ShaderController
    glUniformBlockBinding(genericShader.ID   , glGetUniformBlockIndex(genericShader.ID, "Matrices"), 0);
    glUniformBlockBinding(genericShader.ID   , glGetUniformBlockIndex(genericShader.ID, "LightBuffer"), 1);

    glUniformBlockBinding(lightShader.ID , glGetUniformBlockIndex(lightShader.ID, "Matrices"), 0);

    glUniformBlockBinding(skyboxShader.ID   , glGetUniformBlockIndex(skyboxShader.ID, "Matrices"), 0);


    // Create a uniform buffer to handle viewprojection and lights
    unsigned int uboMatrices;
    glGenBuffers(1, &uboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

    glGenBuffers(1, &uboLights);
    glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
    glBufferData(GL_UNIFORM_BUFFER, lightPositions.size() * sizeof(Light), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, uboLights);

    objectManager.uboLights = uboLights;
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

    // Framebuffer for normal color output
    FrameBuffer colorFB("Color Pass", SCR_WIDTH, SCR_HEIGHT);
    FrameBuffer postprocessFB("Post Process Pass", SCR_WIDTH, SCR_HEIGHT);
    renderPasses.push_back(colorFB);
    renderPasses.push_back(postprocessFB);

    // TODO: refactor later
    // Need to figure out how to organize and save scenes
    for (int i = 0; i < 5; ++i)
    {
        std::string name = "Cube " + std::to_string(i);
        Cube* cube = new Cube();
        cube->name = name;
        cube->shader = &genericShader;
        cube->texture = tex2;
        cube->position = boxPositions[i];
        objectManager.Add(cube);

        Light* light = new Light();
        light->name = lightNames[i];
        light->shader = &lightShader;
        light->color = glm::vec4(lightColors[i], 1.0f);
        light->position = lightPositions[i];
        light->scale = glm::vec3(0.5f);
        objectManager.Add(light);
    }

    // Testing save scene
    loader.SaveScene(objectManager, "");
    return 0;

    Model nanosuit("Models/nanosuit/nanosuit.obj");
    nanosuit.shader = shaderController.Get("generic");
    nanosuit.name = "Nanosuit";
    nanosuit.scale = glm::vec3(0.2f);
    objectManager.Add(&nanosuit);

    // ===================================================================
    // Rendering Loop
    while (glfwWindowShouldClose(mWindow) == false)
    {
        std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
        // per-frame time logic
        // TODO replace with high resolution clock
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(mWindow);

        // TODO
        // Set camera based off game state
        // if GAME is playing then use game camera
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 proj = glm::mat4(1.0f);
        if (GAME.state == PLAY || GAME.state == PAUSE)
        {
            view = gameCamera.GetViewMatrix();
            proj = gameCamera.GetProjMatrix((float)SCR_WIDTH, (float)SCR_HEIGHT);
        }
        else
        {
            view = camera.GetViewMatrix();
            proj = camera.GetProjMatrix((float)SCR_WIDTH, (float)SCR_HEIGHT);
        }

        // Send the view and projection matrices to the UBO
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(proj));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        if (tentGui.isEnabled)
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGuizmo::BeginFrame();

            ImGui::ShowDemoWindow();
        }

        // TODO Update physics of all rigidbodies


        // Rendering step

        // ===================================================================
        { // First render pass
            // Getting color of the scene
            glBindFramebuffer(GL_FRAMEBUFFER, colorFB.ID);

            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            // =====================================
            //{ // Background
            //    glDepthMask(GL_FALSE);
            //    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture.ID);
            //    skybox.shader->use();
            //    glBindVertexArray(skybox.VAO);
            //    glDrawArrays(GL_TRIANGLES, 0, 36);
            //    glBindVertexArray(0);
            //    glDepthMask(GL_TRUE);
            //}

            // Background Fill Color
            glEnable(GL_DEPTH_TEST);

            // Draw scene
            objectManager.Draw();

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        // ===================================================================
        // TODO: depth pass for shadowmaps
        // second pass
        // Get depth information for shadows
//        {
//            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
//            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
//            glClear(GL_DEPTH_BUFFER_BIT);
//            int i = 0;
//            for (auto renderObject : objectManager.objectList)
//            {
//                renderObject->Draw(tex1, boxPositions[i]);
//                ++i;
//            }
//            glBindFramebuffer(GL_FRAMEBUFFER, 0);
//            glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
//        }

        // ===================================================================
        { // Final pass: post-process
            screenQuad.texture = colorFB.texture;
            glBindFramebuffer(GL_FRAMEBUFFER, postprocessFB.ID);

            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glDisable(GL_DEPTH_TEST);
            screenQuad.Draw();

            // TODO how to handle the need to
            // still render to the default FB to
            // show something on the screen?
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glDisable(GL_DEPTH_TEST);
            screenQuad.Draw();
        }

        // ===================================================================

        if (tentGui.isEnabled)
        {
            std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> timeSpan = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);

            tentGui.ShowMetrics(timeSpan.count());
            tentGui.RenderStateButtons(GAME);
            tentGui.ShowCamera(camera);
            tentGui.ShowCamera(gameCamera);
            tentGui.ShowRenderPasses(renderPasses);
            tentGui.RenderGUI(objectManager);
        }

        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
        glfwPollEvents();

    } // End render loop

    // Cleanup for imgui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // TODO later delete all renderpasses stored in some list
    for (auto framebuffers : renderPasses)
    {
        glDeleteFramebuffers(1, &framebuffers.ID);
    }

    // TODO?
    // Clean up all objects in scene
    
//    for (auto object : objectManager.objectList)
//    {
//        delete(object);
//        //objects = nullptr;
//    }
    
    glfwTerminate();
    return EXIT_SUCCESS;
}


void processInputOnce(GLFWwindow *window)
{

}

// TODO move this into an input handler
// maybe need shared data update all necessary info
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    ImGuiIO& io = ImGui::GetIO();
    // Dont process any keyinputs when writing text into imgui
    if (io.WantCaptureKeyboard)
    {
        return;
    }

    // Quit
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        // TODO add a prompt to ask user if they really want to exit
        glfwSetWindowShouldClose(window, true);
    }

    // Camera movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    // Camera speed increase
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        camera.MovementSpeed = camera.maxSpeed;
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
    {
        camera.MovementSpeed = camera.normalSpeed;
    }

    // TODO move to input handler so that
    // theres a more reusable way to have this work only once per click
    // Hot reload shaders only once per click
    {
        static int oldState = GLFW_RELEASE;
        int newState = glfwGetKey(window, GLFW_KEY_R);
        if (newState == GLFW_PRESS && oldState == GLFW_RELEASE)
        {
            shaderController.ReloadShaders();
        }
        oldState = newState;
    }

    // Enable/Disable IMGUI once per click
    {
        static int oldState1 = GLFW_RELEASE;
        int newState1 = glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT);
        if (newState1 == GLFW_PRESS && oldState1 == GLFW_RELEASE)
        {
            tentGui.isEnabled ^= 1;
        }
        oldState1 = newState1;
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

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    ImGuiIO& io = ImGui::GetIO();
    if (!io.WantCaptureMouse)
    {
        camera.ProcessMouseScroll(yoffset);
    }
}
