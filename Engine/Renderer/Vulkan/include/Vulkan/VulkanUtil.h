#include <Vulkan/vulkan.hpp>

namespace vk::util {

    uint32_t FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);

    void CreateBuffer(uint32_t size, vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags propertyFlags, vk::UniqueBuffer& buffer, vk::UniqueDeviceMemory& bufferMemory);

    void CopyBuffer(vk::UniqueBuffer& srcBuffer, vk::UniqueBuffer& dstBuffer, uint32_t size);

    vk::CommandBuffer BeginSingleTimeCommands();

    void EndSingleTimeCommands(vk::CommandBuffer commandBuffer);

    void SwitchImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

    vk::UniqueImage CreateUniqueImage(
        uint32_t width, uint32_t height,
        vk::Format format,
        vk::ImageTiling tiling,
        vk::ImageUsageFlags usage
    );

    vk::UniqueDeviceMemory CreateUniqueDeviceMemory(
        vk::Image image,
        vk::MemoryPropertyFlags properties
    );

    vk::UniqueImageView CreateUniqueImageView(
        vk::Image image,
        vk::Format format,
        vk::ImageAspectFlagBits aspectFlags
    );

    vk::Format FindSupportedFormat(
        const std::vector<vk::Format>& formats,
        vk::ImageTiling tiling,
        vk::FormatFeatureFlagBits features
    );

    vk::Format FindDepthFormat();

    bool HasStencilComponent(vk::Format format);

    std::vector<vk::DescriptorSetLayout> ConvertUnique(
        const std::vector<vk::UniqueDescriptorSetLayout>& uniqueLayouts
    );

}
