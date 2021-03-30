#pragma once

#include "Renderer/GraphicsContext.h"

struct GLFWwindow;

class OpenGLContext : public GraphicsContext
{
public:
    OpenGLContext(GLFWwindow* windowHandle);

    virtual void Init() override;
    virtual void SwapBuffers() override;

private:
    GLFWwindow* windowHandle;
};
