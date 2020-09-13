#include <GLFW/glfw3.h>

#include "Renderer/Renderer.h"
#include "Renderer/GraphicsContext.h"
#include "Platform/OpenGL/OpenGLContext.h"
#include "Platform/Vulkan/VulkanContext.h"

std::unique_ptr<GraphicsContext> GraphicsContext::Create(void* window)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::OpenGL:
            return CreateUniquePtr<OpenGLContext>(static_cast<GLFWwindow*>(window));
        case RendererAPI::API::Vulkan:
            return CreateUniquePtr<VulkanContext>(static_cast<GLFWwindow*>(window));
    }
}
