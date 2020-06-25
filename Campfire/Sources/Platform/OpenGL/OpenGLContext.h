#ifndef OPENGL_CONTEXT_H
#define OPENGL_CONTEXT_H

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

#endif // OPENGL_CONTEXT_H
