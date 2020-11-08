#include "Renderer/GraphicsContext.h"
#include "OpenGL/OpenGLContext.h"
#include "Core/Log.h"

std::unique_ptr<GraphicsContext> GraphicsContext::Create(void* window)
{
    // TODO create context based on which API is being used
    //switch (Renderer::GetAPI())
    //{
    //    case RendererAPI::API::OpenGL: return CreateScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
    //}

    return CreateUniquePtr<OpenGLContext>(static_cast<GLFWwindow*>(window));
}
