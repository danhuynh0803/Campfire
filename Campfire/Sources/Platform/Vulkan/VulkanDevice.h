#pragma once

#include <vulkan/vulkan.hpp>

enum class QueueFamilyType
{
    GRAPHICS,
    PRESENT,
    TRANSFER
};

class VulkanDevice
{
public:
    VulkanDevice();

    vk::PhysicalDevice GetVulkanPhysicalDevice() { return physicalDevice; }
    vk::Device GetVulkanDevice() { return device.get(); }

    // TODO have just one function with enums to select which family/queue
    vk::Queue GetGraphicsQueue() { return graphicsQueue; }
    vk::Queue GetPresentQueue() { return presentQueue; }

    size_t GetQueueFamilyIndex(QueueFamilyType type)
    {
        switch (type)
        {
            case QueueFamilyType::GRAPHICS: return graphicsQueueFamilyIndex;
            case QueueFamilyType::PRESENT: return presentQueueFamilyIndex;
            //case QueueFamilyType::TRANSFER: return graphicsQueueFamilyIndex;
        }
    }

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
    vk::Queue graphicsQueue;
    vk::Queue presentQueue;
};
