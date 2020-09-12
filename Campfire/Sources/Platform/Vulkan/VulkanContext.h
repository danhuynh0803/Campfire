#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>

class VulkanContext
{
public:
    VulkanContext();
    ~VulkanContext();

private:
    vk::Instance CreateInstance();
    vk::PhysicalDevice GetPhysicalDevice();
    bool IsDeviceSuitable(vk::PhysicalDevice device);

private:
    // TODO change to uniqueInstance?
    vk::Instance instance;

    std::vector<vk::PhysicalDevice> physicalDevices;
    vk::PhysicalDevice physicalDevice;

    std::vector<vk::QueueFamilyProperties> queueFamilyProperties;

    //vk::PhysicalDeviceProperties deviceProperties;
    //vk::PhysicalDeviceFeatures deviceFeatures;
    //vk::PhysicalDeviceMemoryProperties deviceMemoryProperties;
    //vks::Version version;
    //vks::Version driverVersion;
};
