#include <glad/glad.h>
#include "OpenGL/OpenGLContext.h"
//#include "Core/Log.h"

OpenGLContext::OpenGLContext(GLFWwindow* window)
    : windowHandle(window)
{
    if (windowHandle == nullptr) {
        LOG_ERROR("Failed to create OpenGL Context");
    }
}

void OpenGLContext::Init()
{
    glfwMakeContextCurrent(windowHandle);
    // TODO list out any other useful information from the context
    //gladLoadGL();

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        LOG_ERROR("Failed to initialize GLAD");
        //return -1;
    }
    //LOG_INFO("OpenGL {0}", glGetString(GL_VERSION));
}

void OpenGLContext::SwapBuffers()
{
    glfwSwapBuffers(windowHandle);
}
