#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "Vulkan/VulkanRenderer.h"
#include "Vulkan/VulkanContext.h"

void VulkanRenderer::DrawIndexed(vk::Buffer vertexBuffer, vk::Buffer indexBuffer, uint32_t count)
{
    auto graphicsPipeline = VulkanContext::Get()->mGraphicsPipeline;
    auto commandBuffer = VulkanContext::Get()->mSwapChain->GetCurrentCommandBuffer();
    auto framebuffer = VulkanContext::Get()->mSwapChain->GetCurrentFramebuffer();
    uint32_t imageIndex = VulkanContext::Get()->mSwapChain->GetCurrentImageIndex();

    vk::CommandBufferBeginInfo beginInfo;
        // TODO: investigate if simulataneous use is faster or slower?
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;
    beginInfo.pInheritanceInfo = nullptr;

    vk::Extent2D extent;
    extent.width = VulkanContext::Get()->mSwapChain->GetWidth();
    extent.height = VulkanContext::Get()->mSwapChain->GetHeight();

    vk::Rect2D renderArea;
    renderArea.offset = {0, 0};
    renderArea.extent = extent;

    commandBuffer.begin(beginInfo);

    // Clear value
    vk::ClearValue clearValues;
    clearValues.color = vk::ClearColorValue(std::array<float, 4>({ { 0.2f, 0.3f, 0.3f, 1.0f } }));
    //clearValues.depthStencil = vk::ClearDepthStencilValue{ 1.0f, 0 };

    vk::RenderPassBeginInfo renderPassBeginInfo;
    renderPassBeginInfo.renderPass = graphicsPipeline->GetVulkanRenderPass();
    renderPassBeginInfo.framebuffer = framebuffer;
    renderPassBeginInfo.renderArea = renderArea;
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = &clearValues;

    commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline->GetVulkanPipeline());

        vk::Buffer vertexBuffers[] = { vertexBuffer };
        vk::DeviceSize offsets[] = { 0 };
        commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);

        auto& descriptorSets = graphicsPipeline->descriptorSets;
        commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, graphicsPipeline->GetVulkanPipelineLayout(), 0, 1, &descriptorSets[imageIndex].get(), 0, nullptr);

        commandBuffer.bindIndexBuffer(indexBuffer, 0, vk::IndexType::eUint32);

        commandBuffer.drawIndexed(count, 1, 0, 0, 0);
    commandBuffer.endRenderPass();

    commandBuffer.end();
}
