#include "Renderer/RendererAPI.h"
#include "Renderer/Framebuffer.h"
#include "OpenGL/OpenGLFramebuffer.h"

SharedPtr<Framebuffer> Framebuffer::Create(uint32_t width, uint32_t height, uint32_t samples)
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None:
            return nullptr;
        case RendererAPI::API::OpenGL:
            return CreateSharedPtr<OpenGLFramebuffer>(width, height, samples);
    }

    return nullptr;
}
