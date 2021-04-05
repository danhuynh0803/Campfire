#pragma once

#include <vulkan/vulkan.hpp>

enum class QueueFamilyType
{
    GRAPHICS = 0,
    PRESENT,
    TRANSFER,
    COMPUTE,
};

class VulkanDevice
{
public:
    VulkanDevice();

    vk::PhysicalDevice GetVulkanPhysicalDevice() { return physicalDevice; }
    vk::Device GetVulkanDevice() { return device.get(); }
    vk::Queue GetQueue(QueueFamilyType type);
    size_t GetQueueFamilyIndex(QueueFamilyType type);
    vk::UniqueCommandPool CreateCommandPool(uint32_t queueFamilyIndex);

private:
    vk::UniqueDevice CreateLogicalDevice();
    vk::PhysicalDevice SelectPhysicalDevice();
    bool IsDeviceSuitable(vk::PhysicalDevice device);

    std::vector<vk::PhysicalDevice> physicalDevices;
    vk::PhysicalDevice physicalDevice;
    vk::UniqueDevice device;

    std::vector<vk::QueueFamilyProperties> queueFamilyProperties;
    size_t graphicsQueueFamilyIndex;
    size_t presentQueueFamilyIndex;
    size_t computeQueueFamilyIndex;
    vk::Queue graphicsQueue;
    vk::Queue presentQueue;
    vk::Queue computeQueue;
};
