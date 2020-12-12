#include "Renderer/RendererAPI.h"
#include "Renderer/Framebuffer.h"
#include "OpenGL/OpenGLFramebuffer.h"

SharedPtr<Framebuffer> Framebuffer::Create(const FramebufferSpec& spec)
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None:
            return nullptr;
        case RendererAPI::API::OpenGL:
            return CreateSharedPtr<OpenGLFramebuffer>(spec);
    }

    return nullptr;
}
