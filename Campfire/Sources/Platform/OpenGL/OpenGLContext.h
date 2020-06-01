#ifndef OPEN_GL_CONTEXT_H
#define OPEN_GL_CONTEXT_H

#include "Renderer/GraphicsContext.h"
#include "GLFW/glfw3.h"

class OpenGLContext : public GraphicsContext
{
public:
    OpenGLContext(GLFWwindow* windowHandle);

    virtual void Init() override;
    virtual void SwapBuffers() override;

private:
    GLFWwindow* windowHandle;
};

#endif // OPEN_GL_CONTEXT_H
