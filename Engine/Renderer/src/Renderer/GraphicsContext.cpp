#include "Renderer/Renderer.h"
#include "Renderer/GraphicsContext.h"
#include "OpenGL/OpenGLContext.h"
#include "Core/Log.h"

std::unique_ptr<GraphicsContext> GraphicsContext::Create(void* window)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::OpenGL:
            return CreateUniquePtr<OpenGLContext>(static_cast<GLFWwindow*>(window));
        //case RendererAPI::API::Vulkan:
            //return CreateUniquePtr<VulkanContext>(static_cast<GLFWwindow*>(window));
    }
}
