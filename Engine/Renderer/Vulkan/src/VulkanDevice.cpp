#include "Vulkan/VulkanDevice.h"
#include "Vulkan/VulkanContext.h"
#include <iostream>

VulkanDevice::VulkanDevice()
{
    physicalDevice = SelectPhysicalDevice();
    device = CreateLogicalDevice();

    graphicsQueue = device->getQueue(graphicsQueueFamilyIndex, 0);
    presentQueue = device->getQueue(presentQueueFamilyIndex, 0);
}

vk::PhysicalDevice VulkanDevice::SelectPhysicalDevice()
{
    physicalDevices = VulkanContext::GetInstance().enumeratePhysicalDevices();
    vk::PhysicalDevice selectedDevice;

    for (const auto& device : physicalDevices)
    {
        if (IsDeviceSuitable(device))
        {
            selectedDevice = device;
            break;
        }
    }
    return selectedDevice;
}

bool VulkanDevice::IsDeviceSuitable(vk::PhysicalDevice device)
{
    vk::PhysicalDeviceProperties deviceProperties = device.getProperties();
    vk::PhysicalDeviceFeatures supportedFeatures = device.getFeatures();

    // TODO add various feature checks
    return supportedFeatures.samplerAnisotropy;
}

vk::UniqueDevice VulkanDevice::CreateLogicalDevice()
{
    // Determine if device contains a graphics queue
    queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

    graphicsQueueFamilyIndex = std::distance(
        queueFamilyProperties.begin(),
        std::find_if(
            queueFamilyProperties.begin(), queueFamilyProperties.end(),
            []( vk::QueueFamilyProperties const& qfp) {
                return qfp.queueFlags & vk::QueueFlagBits::eGraphics;
            }
        )
    );

    assert( graphicsQueueFamilyIndex < queueFamilyProperties.size() );

    computeQueueFamilyIndex = std::distance(
        queueFamilyProperties.begin(),
        std::find_if(
            queueFamilyProperties.begin(), queueFamilyProperties.end(),
            [](vk::QueueFamilyProperties const& qfp) {
                return qfp.queueFlags & vk::QueueFlagBits::eCompute;
            }
        )
    );

    presentQueueFamilyIndex = graphicsQueueFamilyIndex;

    if ((graphicsQueueFamilyIndex == queueFamilyProperties.size()) ||
        (presentQueueFamilyIndex == queueFamilyProperties.size()))
    {
        throw std::runtime_error("Could not find a queue for graphics or present -> terminating");
    }

    float queuePriority = 0.0f;
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo;
    deviceQueueCreateInfo.flags = vk::DeviceQueueCreateFlags();
    deviceQueueCreateInfo.queueFamilyIndex = static_cast<uint32_t>(graphicsQueueFamilyIndex);
    deviceQueueCreateInfo.queueCount = 1;
    deviceQueueCreateInfo.pQueuePriorities = &queuePriority;

    const std::vector<const char*> deviceExtensions =
    {
        "VK_KHR_swapchain"
    };

    vk::DeviceCreateInfo deviceInfo {};
    deviceInfo.flags = vk::DeviceCreateFlags();
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
    deviceInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();

    vk::PhysicalDeviceFeatures deviceFeatures {};
    deviceFeatures.samplerAnisotropy = true;
    deviceInfo.pEnabledFeatures = &deviceFeatures;

    return physicalDevice.createDeviceUnique(deviceInfo);
}


