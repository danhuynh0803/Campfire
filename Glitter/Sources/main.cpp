// Local Headers
#include "glitter.hpp"

// ImGui headers
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

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

// My headers
#include "Shader.h"
#include "ShaderController.h"
#include "Camera.h"
#include "Light.h"
#include "Texture.h"
#include "ObjectManager.h"
#include "Cube.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void RenderGUI();

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;
float texMix = 0.2f;

void RenderLights(Shader);

// TODO: move buffers to their own classes at some point
unsigned int VAO;
unsigned int uboLights;
Texture tex1, tex2;
std::vector<Light> lights;
std::vector<glm::vec3> boxPositions;

ShaderController shaderController;

ObjectManager objectManager;
ObjectManager lightManager;

bool IMGUI_ENABLED = 1;

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
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
    ImGui_ImplOpenGL3_Init("#version 450");
    // Setup Dear ImGui style;
    ImGui::StyleColorsDark();

    // TODO reformat this to use glTF or some other more manageable way
    // Vertex info for a cube
    float quadVertices[] =
    {
        -1,  1,  0,     0.0f, 1.0f,
        -1, -1,  0,     0.0f, 0.0f,
         1, -1,  0,     1.0f, 0.0f,
         1, -1,  0,     1.0f, 0.0f,
         1,  1,  0,     1.0f, 1.0f,
        -1,  1,  0,     0.0f, 1.0f
    };

    // use our shader program when we want to render an object
    Shader lightShader("../Glitter/Shaders/light.vert", "../Glitter/Shaders/light.frag");
    Shader boxShader("../Glitter/Shaders/box.vert", "../Glitter/Shaders/box.frag");
    Shader screenShader("../Glitter/Shaders/postProcess.vert", "../Glitter/Shaders/kernel.frag");
    // Add shader to shaderController for hot reloading
    // TODO handle this seamlessly so that theres no need to add shader each time to controller
    shaderController.Add(&lightShader);
    shaderController.Add(&boxShader);
    shaderController.Add(&screenShader);

    // ===================================================================
    // Setup for textures
    //
    tex1 = Texture("Textures/revue.jpg");
    //tex1 = Texture("Textures/uv.png");
    tex2 = Texture("Textures/awesomeface.png");
    // ===================================================================

    // Create quadVAO
    unsigned int quadVAO;
    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);

    unsigned int quadVBO;
    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // TODO replace this into some more manageable format, like glTF
    // ===================================================================
    // Light information
    //
    std::vector<glm::vec3> lightPositions = {
        glm::vec3( 1.0f,  0.0f,  0.0f),
        glm::vec3(-1.0f,  1.0f,  0.0f),
        glm::vec3(-1.0f,  0.0f,  0.0f),
        glm::vec3( 1.0f,  0.0f,  0.0f),
        glm::vec3( 0.0f, -1.0f,  0.0f),
    };

    std::vector<glm::vec3> lightColors = {
        glm::vec3(1.0f, 0.3f, 0.3f), // red
        glm::vec3(0.0f, 1.0f, 0.0f), // green
        glm::vec3(0.0f, 0.0f, 1.0f), // blue
        glm::vec3(1.0f, 1.0f, 0.0f), // yellow
        glm::vec3(0.0f, 1.0f, 1.0f), // purple
    };

    for (int i = 0; i < lightPositions.size(); ++i)
    {
        lights.push_back(
            Light(glm::vec4(lightPositions[i], 1.0f), glm::vec4(lightColors[i], 1.0f))
        );
    }

    boxPositions = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 0.1f,  0.0f, -1.5f),
        glm::vec3(-3.5f,  2.5f, -4.0f),
        glm::vec3(-2.4f, -0.5f, -5.0f),
        glm::vec3( 1.5f, -1.0f, -7.0f)
    };

    // ===================================================================
    // Bind UBO block index to shaders
    glUniformBlockBinding(boxShader.ID   , glGetUniformBlockIndex(boxShader.ID, "Matrices"), 0);
    glUniformBlockBinding(lightShader.ID , glGetUniformBlockIndex(lightShader.ID, "Matrices"), 0);
    glUniformBlockBinding(boxShader.ID   , glGetUniformBlockIndex(boxShader.ID, "LightBuffer"), 1);

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
    // Frame buffers for various render passes
    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    // To complete a framebuffer we need:
    // 1. attach at least one buffer (color, depth, or stencil)
    // 2. should at least have one color attachment
    // 3. attachments should be complete
    // 4. buffers should have the same number of samples
    // NOTE: if sampling data then use textures
    // else use renderbuffers

    // Attach a texture to fbo
    unsigned int fbTexture;
    glGenTextures(1, &fbTexture);
    glBindTexture(GL_TEXTURE_2D, fbTexture);
    // Texture will be filled when we render to framebuffer
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    // Attach texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbTexture, 0);

    // Create rbo for depth and stencil
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    // Attach rbo
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer not complete!\n";
        // Exit?
    }
    // Bind framebuffer if successful
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //glDeleteFramebuffers(1, &fbo);

    // TODO: refactor later
    for (int i = 0; i < 5; ++i)
    {
        std::string name = "Cube " + std::to_string(i);
        objectManager.Add(new Cube(name, boxShader));
        lightManager.Add(new Cube("light", lightShader));
    }

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

        if (IMGUI_ENABLED)
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::ShowDemoWindow();
        }

        // ===================================================================
        // First render pass
        // Getting color of the scene
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        // Background Fill Color
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        // Draw scene
        int i = 0;
        for (auto renderObject : objectManager.objectList)
        {
            renderObject->Draw(tex1, boxPositions[i]);
            ++i;
        }

        i = 0;
        for (auto lightObject : lightManager.objectList)
        {
            glm::vec3 newPos = lights[i].pos;
            glm::vec3 scale(0.5f);
            if (i == 0)
            {
                float radius = 5.0f;
                float omega = 1.0f;

                newPos += radius * glm::vec3(cos(omega * glfwGetTime()),
                        0.0f,
                        sin(omega * glfwGetTime()));
            }

            glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
            // Send in light info to light UBO
            glBufferSubData(GL_UNIFORM_BUFFER,
                    2*sizeof(glm::vec4)*i,
                    sizeof(glm::vec4),
                    glm::value_ptr(newPos));

            glBufferSubData(GL_UNIFORM_BUFFER,
                    2*sizeof(glm::vec4)*i + sizeof(glm::vec4),
                    sizeof(glm::vec4),
                    glm::value_ptr(lights[i].color));

            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            glBindBufferBase(GL_UNIFORM_BUFFER, 1, uboLights);

            glUniform3fv(glGetUniformLocation(lightShader.ID, "lightColor"), 1, glm::value_ptr(lights[i].color));

            lightObject->Draw(tex1, newPos, scale);
            ++i;
        }

        //RenderLights(lightShader);

        // ===================================================================
// TODO: depth pass for shadowmaps
//        // second pass
//        // Get depth information for shadows
//        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
//        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
//            glClear(GL_DEPTH_BUFFER_BIT);
//            RenderScene(boxShader);
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
//        glBindTexture(GL_TEXTURE_2D, depthMap);

        // ===================================================================
        // Final pass: post-process
        // Return to default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        screenShader.use();
        glBindVertexArray(quadVAO);
        glDisable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, fbTexture);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // ===================================================================

        if (IMGUI_ENABLED)
        {
            RenderGUI();
        }

        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
        glfwPollEvents();

    } // End render loop

    // Cleanup for imgui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteFramebuffers(1, &fbo);
    glfwTerminate();
    return EXIT_SUCCESS;
}

void RenderGUI()
{
    ImGui::Begin("Scene Hierarchy");
    if (ImGui::Button("Reload Shaders"))
    {
        shaderController.ReloadShaders();
    }
    ImGui::Spacing();

    ImGui::Separator();
    // List all GLObjects within scene
    if (ImGui::TreeNode("Scene Objects"))
    {
        int i = 0;
        for (auto object : objectManager.objectList)
        {
            if (ImGui::TreeNode((void*)(intptr_t)i, "%s", object->name.c_str()))
            {
                ImGui::Text("Texture: %s", tex1.GetName().c_str());
                ImGui::Text("Size: %d x %d", tex1.width, tex1.height);
                ImGui::Image((void*)(intptr_t)tex1.ID, ImVec2(128, 128));
                ImGui::Separator();
                ImGui::TreePop();
            }
            ++i;
        }
        ImGui::TreePop();
    }
    ImGui::End();

    ImGui::Begin("Metrics");
    static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
    ImGui::PlotLines("Frame Times", arr, IM_ARRAYSIZE(arr));
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// TODO delete later
//void RenderLights(Shader lightShader)
//{
//    // Draw our lights
//    lightShader.use();
//    for (int i = 0; i < lights.size(); ++i)
//    {
//        glm::vec3 newPos = lights[i].pos;
//        // first light is moving light
//        if (i == 0)
//        {
//            float radius = 5.0f;
//            float omega = 1.0f;
//
//            newPos += radius * glm::vec3(cos(omega * glfwGetTime()),
//                    0.0f,
//                    sin(omega * glfwGetTime()));
//        }
//
//        glm::mat4 model = glm::mat4(1.0f);
//        model = glm::translate(model, newPos);
//        model = glm::scale(model, glm::vec3(0.5f));
//
//        glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
//        // Send in light info to light UBO
//        glBufferSubData(GL_UNIFORM_BUFFER,
//                2*sizeof(glm::vec4)*i,
//                sizeof(glm::vec4),
//                glm::value_ptr(newPos));
//
//        glBufferSubData(GL_UNIFORM_BUFFER,
//                2*sizeof(glm::vec4)*i + sizeof(glm::vec4),
//                sizeof(glm::vec4),
//                glm::value_ptr(lights[i].color));
//
//        glBindBuffer(GL_UNIFORM_BUFFER, 0);
//        glBindBufferBase(GL_UNIFORM_BUFFER, 1, uboLights);
//
//        lightShader.use();
//        // send in model
//        glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
//
//        // Pass in light color to shader
//        glUniform3fv(glGetUniformLocation(lightShader.ID, "lightColor"), 1, glm::value_ptr(lights[i].color));
//
//        // Draw the object
//        glBindVertexArray(VAO);
//        glDrawArrays(GL_TRIANGLES, 0, 36);
//        glBindVertexArray(0);
//    }
//}

// TODO move this into an input handler
// maybe need shared data update all necessary info
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    // Quit
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

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
        camera.MovementSpeed = maxSpeed;
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
    {
        camera.MovementSpeed = normalSpeed;
    }

    // TODO move to input handler so that
    // theres a more reusable way to have this work only once per click
    // Hot reload shaders only once per click
    static int oldState = GLFW_RELEASE;
    int newState = glfwGetKey(window, GLFW_KEY_R);
    if (newState == GLFW_PRESS && oldState == GLFW_RELEASE)
    {
        shaderController.ReloadShaders();
    }
    oldState = newState;

    // Enable/Disable IMGUI once per click
    static int oldState1 = GLFW_RELEASE;
    int newState1 = glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT);
    if (newState1 == GLFW_PRESS && oldState1 == GLFW_RELEASE)
    {
        IMGUI_ENABLED ^= 1;
    }
    oldState1 = newState1;
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
