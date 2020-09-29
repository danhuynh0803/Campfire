#include "VulkanFramebuffer.h"

VulkanFramebuffer::VulkanFramebuffer(uint32_t width, uint32_t height)
{
    vk::FramebufferCreateInfo framebufferInfo;
    framebufferInfo.flags = vk::FramebufferCreateFlags();
    framebufferInfo.renderPass = renderPass.get();
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = width;
    framebufferInfo.height = height;
    framebufferInfo.layers = 1;

    framebuffer = device->createFramebufferUnique(framebufferInfo);
}
