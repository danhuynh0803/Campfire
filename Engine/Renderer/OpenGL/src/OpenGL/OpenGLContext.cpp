#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "OpenGL/OpenGLContext.h"

OpenGLContext::OpenGLContext(GLFWwindow* window)
    : windowHandle(window)
{
    if (windowHandle == nullptr) {
        CORE_ERROR("Failed to create OpenGL Context");
    }
}

void OpenGLContext::Init()
{
    glfwMakeContextCurrent(windowHandle);
    // TODO list out any other useful information from the context
    //gladLoadGL();

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        CORE_ERROR("Failed to initialize GLAD");
        //return -1;
    }
    //LOG_INFO("OpenGL {0}", glGetString(GL_VERSION));
}

void OpenGLContext::SwapBuffers()
{
    glfwSwapBuffers(windowHandle);
}
