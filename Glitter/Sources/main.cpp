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

// My headers
#include "Shader.h"
#include "ShaderController.h"
#include "Camera.h"
#include "Light.h"
#include "Texture.h"
#include "ObjectManager.h"
#include "FrameBuffer.h"
#include "Cube.h"
#include "Quad.h"

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

// TODO: move buffers to their own classes at some point
unsigned int VAO;
unsigned int uboLights;
std::vector<Light> lights;
std::vector<glm::vec3> boxPositions;

ShaderController shaderController;

ObjectManager objectManager;
ObjectManager lightManager;
std::vector<FrameBuffer> renderPasses;

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


    // use our shader program when we want to render an object
    Shader genericShader("../Glitter/Shaders/generic.vert", "../Glitter/Shaders/generic.frag");
    Shader lightShader("../Glitter/Shaders/light.vert", "../Glitter/Shaders/light.frag");
    Shader screenShader("../Glitter/Shaders/postProcess.vert", "../Glitter/Shaders/kernel.frag");
    // Add shader to shaderController for hot reloading
    // TODO handle this seamlessly so that theres no need to add shader each time to controller
    shaderController.Add(&genericShader);
    shaderController.Add(&lightShader);
    shaderController.Add(&screenShader);

    // ===================================================================
    // Setup for textures
    //
    Texture tex1("Textures/wall.jpg");
    Texture tex2("Textures/awesomeface.png");
    // ===================================================================

    Quad screenQuad;
    screenQuad.shader = screenShader;

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

    // TODO move model info into GlObjects
    boxPositions = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 0.1f,  0.0f, -1.5f),
        glm::vec3(-3.5f,  2.5f, -4.0f),
        glm::vec3(-2.4f, -0.5f, -5.0f),
        glm::vec3( 1.5f, -1.0f, -7.0f),
    };

    // ===================================================================
    // Bind UBO block index to shaders
    glUniformBlockBinding(genericShader.ID   , glGetUniformBlockIndex(genericShader.ID, "Matrices"), 0);
    glUniformBlockBinding(lightShader.ID , glGetUniformBlockIndex(lightShader.ID, "Matrices"), 0);
    glUniformBlockBinding(genericShader.ID   , glGetUniformBlockIndex(genericShader.ID, "LightBuffer"), 1);

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

    // Framebuffer for normal color output
    FrameBuffer colorFB("Color Pass", SCR_WIDTH, SCR_HEIGHT);
    FrameBuffer postprocessFB("Post Process Pass", SCR_WIDTH, SCR_HEIGHT);
    renderPasses.push_back(colorFB);
    renderPasses.push_back(postprocessFB);

    // TODO: refactor later
    for (int i = 0; i < 5; ++i)
    {
        std::string name = "Cube " + std::to_string(i);
        Cube* cube = new Cube();
        cube->name = name;
        cube->shader = genericShader;
        cube->texture = tex2;
        cube->position = boxPositions[i];
        objectManager.Add(cube);

        Cube* light = new Cube();
        light->name = "light";
        light->shader = lightShader;
        light->position = lightPositions[i];
        light->scale = glm::vec3(0.5f);
        lightManager.Add(light);
    }

    Quad floor;
    floor.name = "Floor Plane";
    floor.shader = genericShader;
    floor.texture = tex1;
    floor.position = glm::vec3(0.0f, -5.0f, 0.0f);
    objectManager.Add(&floor);

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
        { // First render pass
            // Getting color of the scene
            glBindFramebuffer(GL_FRAMEBUFFER, colorFB.ID);
            // Background Fill Color
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);

            // Draw scene
            int i = 0;
            for (auto renderObject : objectManager.objectList)
            {
                if (renderObject->isActive)
                {
                    renderObject->Draw();
                }
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

                lightObject->shader.use();
                glUniform3fv(glGetUniformLocation(lightShader.ID, "lightColor"), 1, glm::value_ptr(lights[i].color));

                lightObject->position = newPos;
                lightObject->Draw(scale);
                ++i;
            }

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

        if (IMGUI_ENABLED)
        {
            // =============================================
            { // Displayer render pass outputs
                ImGui::Begin("Render Passes Outputs");
                if (ImGui::TreeNode("Render Passes"))
                {
                    for (int i = 0; i < renderPasses.size(); ++i)
                    {
                        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                        if (ImGui::TreeNode((void*)(intptr_t)i, "%s", renderPasses[i].name))
                        {
                            ImGui::Image((void*)(intptr_t)renderPasses[i].texture.ID, ImVec2(SCR_WIDTH/4, SCR_HEIGHT/4), ImVec2(0,1), ImVec2(1,0));
                            ImGui::Separator();
                            ImGui::TreePop();
                        }
                    }
                    ImGui::TreePop();
                }
                ImGui::End();
            }

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

    // TODO later delete all renderpasses stored in some list
    for (auto framebuffers : renderPasses)
    {
        glDeleteFramebuffers(1, &framebuffers.ID);
    }

    // TODO?
    // Clean up all objects in scene
    /*
    for (auto object : objectManager.objectList)
    {
        delete(object);
        //objects = nullptr;
    }
    */

    glfwTerminate();
    return EXIT_SUCCESS;
}

void ShowMetrics()
{
    ImGui::Begin("Metrics");
    static bool animate = true;
    ImGui::Checkbox("Animate", &animate);

    static float values[90] = {};
    static int values_offset = 0;
    static double refresh_time = 0.0;
    if (!animate || refresh_time == 0.0)
        refresh_time = ImGui::GetTime();

    while (refresh_time < ImGui::GetTime()) // Create dummy data at fixed 60 hz rate for the demo
    {
        static float phase = 0.0f;
        values[values_offset] = cosf(phase);
        values_offset = (values_offset+1) % IM_ARRAYSIZE(values);
        phase += 0.10f*values_offset;
        refresh_time += 1.0f/60.0f;
    }

    // Display Frame window
    {
        float average = 0.0f;
        for (int n = 0; n < IM_ARRAYSIZE(values); n++)
            average += values[n];
        average /= (float)IM_ARRAYSIZE(values);
        char overlay[32];
        sprintf(overlay, "avg %f", average);
        ImGui::PlotLines("Frame times", values, IM_ARRAYSIZE(values), values_offset, overlay, -1.0f, 1.0f, ImVec2(0,80));
    }

    ImGui::End();
}

enum Geometry { CUBE, QUAD, SPHERE, NONE };

#include <stdlib.h>
void LoadObject(Geometry geom, std::string name, float pos[3], float rot[3], float scale[3])
{
    Shader genericShader("../Glitter/Shaders/generic.vert", "../Glitter/Shaders/generic.frag");
    Texture tempTex("Textures/wall.jpg");

    GlObject* object;
    switch (geom)
    {
        case CUBE: object = new Cube(); break;
        case QUAD: object = new Quad(); break;
        //case SPHERE: static_cast<Cube*>(object); break;
        case NONE: return;
        default: return;
    }

    object->name = name;
    object->shader = genericShader;
    object->texture = tempTex;
    object->position = glm::make_vec3(pos);
    object->rotation = glm::make_vec3(rot);
    object->scale = glm::make_vec3(scale);

    objectManager.Add(object);
}

void DeleteObject(int index)
{
    if (!objectManager.objectList.empty())
    {
        objectManager.objectList.erase(objectManager.objectList.begin() + index);
    }
}

void DeleteObject(GlObject *object)
{
    // TODO
    // Find object and delete it
}

void ShowPrimitiveGenerator()
{
    static int objectIdx = -1;
    const char* objects[] = {"Cube", "Quad", "Sphere"};
    static Geometry selectedGeom = NONE;

    ImGui::Text("Generate Primitive");
    if (ImGui::Button("Geometry"))
    {
        ImGui::OpenPopup("selectPopup");
    }
    ImGui::SameLine();
    ImGui::TextUnformatted(objectIdx == -1 ? "<None>" : objects[objectIdx]);
    if (ImGui::BeginPopup("selectPopup"))
    {
        ImGui::Text("Primitives");
        ImGui::Separator();
        for (int i = 0; i < IM_ARRAYSIZE(objects); i++)
        {
            if (ImGui::Selectable(objects[i]))
            {
                objectIdx = i;
                selectedGeom = static_cast<Geometry>(i);
            }
        }

        ImGui::EndPopup();
    }

    static char tag[128] = "";
    ImGui::InputTextWithHint("Tag", "Enter object tag here", tag, IM_ARRAYSIZE(tag));

    static float position[3] = { 0.0f, 0.0f, 0.0f };
    ImGui::DragFloat3("Position", position, 0.01f);
    ImGui::Spacing();

    static float rotation[3] = { 0.0f, 0.0f, 0.0f };
    ImGui::DragFloat3("Rotation", rotation, 0.01f);
    ImGui::Spacing();

    static float scale[3] = { 1.0f, 1.0f, 1.0f };
    ImGui::DragFloat3("Scale", scale, 0.01f);
    ImGui::Spacing();

    std::string tagString(tag);
    if (ImGui::Button("Generate"))
    {
        LoadObject(selectedGeom, tagString, position, rotation, scale);
    }
}

void ShowInspector(GlObject* object)
{
    // Show and be able to modify information on selected object
    ImGui::Begin("Inspector");

    // TODO highlight the selected object in the scene
    {

    }

    { // TODO Draw object local coordinates to show orientation

    }

    ImGui::Checkbox("", &object->isActive);
    ImGui::SameLine();
    // TODO: cleaner way of doing this somehow?
    char tag[128];
    strcpy(tag, object->name.c_str());
    ImGui::InputText("Tag", tag, 128);
    object->name.assign(tag);

    { // Transform info
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Transform"))
        {
            float position[3] = {
                object->position.x,
                object->position.y,
                object->position.z
            };
            ImGui::DragFloat3("Position", position, 0.01f);
            object->position = glm::make_vec3(position);
            ImGui::Spacing();

            float rotation[3] = {
                object->rotation.x,
                object->rotation.y,
                object->rotation.z
            };
            ImGui::DragFloat3("Rotation", rotation, 0.1f);
            object->rotation = glm::make_vec3(rotation);
            ImGui::Spacing();

            float scale[3] = {
                object->scale.x,
                object->scale.y,
                object->scale.z
            };
            ImGui::DragFloat3("Scale", scale, 0.01f);
            object->scale = glm::make_vec3(scale);
            ImGui::TreePop();
        }
        ImGui::Separator();
    }

    { // Mesh details
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Mesh Details"))
        {
            // TODO allow loading up new texture and shader
            Texture objectTex = object->texture;
            ImGui::Text("Tex: %s", objectTex.GetName().c_str());
            ImGui::Text("Dim: %dx%d", objectTex.width, objectTex.height);
            ImGui::Image((void*)(intptr_t)objectTex.ID, ImVec2(128, 128), ImVec2(0,1), ImVec2(1,0));

            ImGui::TreePop();
        }
        ImGui::Separator();
    }

    { // TODO what other details to add?

    }

    ImGui::End();
}

void ShowObjects(ObjectManager manager)
{
    static int selected = -1;

    ImGui::Text("Scene Objects");
    // TODO
    // Note: the number 105 was just chosen by eye
    // Eventually figure out a way that takes into account the button's width
    ImGui::SameLine(ImGui::GetWindowWidth()-105);
    if (ImGui::Button("Delete Object") && selected != -1)
    {
        DeleteObject(selected);
        selected = -1;
    }

    int i = 0;
    for (auto object : objectManager.objectList)
    {
        char buf[32];
        sprintf(buf, "Idx:%d\t Tag:%s", i, object->name.c_str());
        if (ImGui::Selectable(buf, selected == i))
        {
            selected = i;
        }
        ++i;
    }

    if (selected != -1)
    {
        ShowInspector(manager.objectList[selected]);
    }
}

void ShowSceneHierarchy()
{
    ImGui::Begin("Scene Hierarchy");

    ShowPrimitiveGenerator();

    ImGui::Separator();

    ShowObjects(objectManager);

    ImGui::Separator();
    //ShowObjects(lightManager);

    ImGui::End();
}


void RenderGUI()
{
    ShowSceneHierarchy();
    ShowMetrics();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
    {
        static int oldState = GLFW_RELEASE;
        int newState = glfwGetKey(window, GLFW_KEY_R);
        if (newState == GLFW_PRESS && oldState == GLFW_RELEASE)
        {
            std::cout << "RELOADING SHADERS\n";
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
            IMGUI_ENABLED ^= 1;
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
