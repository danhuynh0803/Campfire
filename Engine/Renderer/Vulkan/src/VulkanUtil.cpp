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
    auto commandBuffer = BeginSingleTimeCommands();
        vk::BufferCopy copyRegion;
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = size;
        commandBuffer.copyBuffer(
            srcBuffer.get(),
            dstBuffer.get(),
            1,
            &copyRegion
        );
    EndSingleTimeCommands(commandBuffer);
}

vk::CommandBuffer BeginSingleTimeCommands()
{
    auto cmdPool = VulkanContext::Get()->GetCommandPool();

    vk::CommandBufferAllocateInfo allocInfo {};
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandPool = cmdPool;
    allocInfo.commandBufferCount = 1;

    // Create a command buffer to process a one time submit command
    auto device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();
    vk::CommandBuffer cmdBuffer;
    device.allocateCommandBuffers(&allocInfo, &cmdBuffer);

    vk::CommandBufferBeginInfo beginInfo {};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    cmdBuffer.begin(beginInfo);

    return cmdBuffer;
}

void EndSingleTimeCommands(vk::CommandBuffer commandBuffer)
{
    commandBuffer.end();

    vk::SubmitInfo submitInfo;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    // TODO maybe optimize by submitting and then batching all data later somehow?
    auto graphicsQueue = VulkanContext::Get()->GetDevice()->GetGraphicsQueue();
    graphicsQueue.submit(1, &submitInfo, nullptr);
    graphicsQueue.waitIdle();
}
