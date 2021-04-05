#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "Vulkan/VulkanRenderer.h"
#include "Vulkan/VulkanContext.h"

vk::CommandBuffer& VulkanRenderer::BeginScene(uint32_t index)
{
    auto graphicsPipeline = VulkanContext::Get()->GetPipeline();
    auto& commandBuffer = VulkanContext::Get()->GetSwapChain()->GetCommandBuffer(index);
    auto framebuffer = VulkanContext::Get()->GetSwapChain()->GetFramebuffer(index);
    VulkanImGuiLayer* vkImguiLayer = Application::Get().imguiLayer;

    vk::CommandBufferBeginInfo beginInfo;
    // TODO: investigate if simulataneous use is faster or slower?
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;
    beginInfo.pInheritanceInfo = nullptr;

    // TODO move this to a beginFrame func
    vk::Extent2D extent;
    extent.width = VulkanContext::Get()->GetSwapChain()->GetWidth();
    extent.height = VulkanContext::Get()->GetSwapChain()->GetHeight();

    vk::Rect2D renderArea;
    renderArea.offset = {0, 0};
    renderArea.extent = extent;

    commandBuffer.begin(beginInfo);

    // Clear value
    std::array<vk::ClearValue, 2> clearValues;
    clearValues[0].color = vk::ClearColorValue(std::array<float, 4>({ { 0.2f, 0.3f, 0.3f, 1.0f } }));
    clearValues[1].depthStencil = vk::ClearDepthStencilValue{ 1.0f, 0 };

    vk::RenderPassBeginInfo renderPassBeginInfo;
    renderPassBeginInfo.renderPass = graphicsPipeline->GetVulkanRenderPass();
    renderPassBeginInfo.framebuffer = framebuffer;
    renderPassBeginInfo.renderArea = renderArea;
    renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassBeginInfo.pClearValues = clearValues.data();

    // TODO replace set index with var
    std::vector<vk::DescriptorSet> descriptorSets {
        graphicsPipeline->GetDescriptorSet(0),
    };

    commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        graphicsPipeline->GetVulkanPipelineLayout(),
        0,
        static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data(),
        0, nullptr
    );
    commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline->GetVulkanPipeline());

    return commandBuffer;

}

void VulkanRenderer::EndScene(vk::CommandBuffer& commandBuffer)
{
    commandBuffer.endRenderPass();
    commandBuffer.end();
}

void VulkanRenderer::DrawIndexed(vk::CommandBuffer& commandBuffer, vk::Buffer vertexBuffer, vk::Buffer indexBuffer, uint32_t count)
{
    vk::Buffer vertexBuffers[] = { vertexBuffer };
    vk::DeviceSize offsets[] = { 0 };
    commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
    commandBuffer.bindIndexBuffer(indexBuffer, 0, vk::IndexType::eUint32);
    commandBuffer.drawIndexed(count, 1, 0, 0, 0);
}
