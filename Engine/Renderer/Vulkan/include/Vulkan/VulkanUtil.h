#include <Vulkan/vulkan.hpp>

uint32_t FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);

void CreateBuffer(uint32_t size, vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags propertyFlags, vk::UniqueBuffer& buffer, vk::UniqueDeviceMemory& bufferMemory);

void CopyBuffer(vk::UniqueBuffer& srcBuffer, vk::UniqueBuffer& dstBuffer, uint32_t size);

vk::CommandBuffer BeginSingleTimeCommands();

void EndSingleTimeCommands(vk::CommandBuffer commandBuffer);
