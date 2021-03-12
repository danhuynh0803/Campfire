#include "Vulkan/VulkanUtil.h"
#include "Vulkan/VulkanContext.h"

uint32_t FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) {

    vk::PhysicalDeviceMemoryProperties memProperties = VulkanContext::Get()->GetDevice()->GetVulkanPhysicalDevice().getMemoryProperties();

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
}

void CreateBuffer(uint32_t size, vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags propertyFlags, vk::UniqueBuffer& buffer, vk::UniqueDeviceMemory& bufferMemory)
{
    vk::Device device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();

    vk::BufferCreateInfo bufferInfo;
    bufferInfo.flags = vk::BufferCreateFlags();
    bufferInfo.size = size;
    bufferInfo.usage = usageFlags;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;

    buffer = device.createBufferUnique(bufferInfo);

    // Set memory requirements
    vk::MemoryRequirements memoryReqs = device.getBufferMemoryRequirements(buffer.get());

    vk::MemoryAllocateInfo allocInfo
    {
        memoryReqs.size,
        FindMemoryType(memoryReqs.memoryTypeBits, propertyFlags),
        //.allocationSize = memoryReqs.size,
        //.memoryTypeIndex = FindMemoryType(memoryReqs.memoryTypeBits, propertyFlags),
    };

    bufferMemory = device.allocateMemoryUnique(allocInfo);

    device.bindBufferMemory(buffer.get(), bufferMemory.get(), 0);
}

void CopyBuffer(vk::UniqueBuffer& srcBuffer, vk::UniqueBuffer& dstBuffer, uint32_t size)
{
    vk::CommandBufferAllocateInfo allocateInfo
    {
        VulkanContext::Get()->mSwapChain->GetCommandPool(),
        vk::CommandBufferLevel::ePrimary,
        1,
        //.commandPool = VulkanContext::Get()->mSwapChain->GetCommandPool(),
        //.level = vk::CommandBufferLevel::ePrimary,
        //.commandBufferCount = 1,
    };

    auto devicePtr = VulkanContext::Get()->GetDevice();
    std::vector<vk::UniqueCommandBuffer> commandBuffer =
        devicePtr->GetVulkanDevice().allocateCommandBuffersUnique(allocateInfo);

    vk::CommandBufferBeginInfo beginInfo
    {
        vk::CommandBufferUsageFlagBits::eOneTimeSubmit
    };

    commandBuffer[0]->begin(beginInfo);
        vk::BufferCopy copyRegion;
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = size;
        commandBuffer[0]->copyBuffer(srcBuffer.get(), dstBuffer.get(), 1, &copyRegion);
    commandBuffer[0]->end();

    vk::SubmitInfo submitInfo;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer[0].get();

    vk::Queue graphicsQueue = devicePtr->GetGraphicsQueue();
    graphicsQueue.submit(1, &submitInfo, nullptr);
    graphicsQueue.waitIdle();
}
