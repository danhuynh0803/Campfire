#include "VulkanDevice.h"
#include "VulkanContext.h"
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
    vk::PhysicalDeviceFeatures deviceFeatures = device.getFeatures();

    // TODO add various feature checks
    return true;
}

vk::UniqueDevice VulkanDevice::CreateLogicalDevice()
{
    // Determine if device contains a graphics queue
    queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

     graphicsQueueFamilyIndex = std::distance (
        queueFamilyProperties.begin(),
        std::find_if (
            queueFamilyProperties.begin(), queueFamilyProperties.end(), []( vk::QueueFamilyProperties const& qfp) {
                return qfp.queueFlags & vk::QueueFlagBits::eGraphics;
            }
        )
    );

    assert( graphicsQueueFamilyIndex < queueFamilyProperties.size() );

    auto surface = VulkanContext::GetSurface();
    // Get queueFamilyIndex that supports present
    // First check if graphicsQueueFamilyIndex is good enough
     presentQueueFamilyIndex =
        physicalDevice.getSurfaceSupportKHR( static_cast<uint32_t>(graphicsQueueFamilyIndex), surface )
            ? graphicsQueueFamilyIndex
            : queueFamilyProperties.size();

    if (presentQueueFamilyIndex == queueFamilyProperties.size())
    {
        // the graphicsQueueFamilyIndex doesn't support present -> look for an other family index that supports both
        // graphics and present
        for (size_t i = 0; i < queueFamilyProperties.size(); i++)
        {
            if ((queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) &&
                physicalDevice.getSurfaceSupportKHR(static_cast<uint32_t>(i), surface))
            {
                graphicsQueueFamilyIndex = i;
                presentQueueFamilyIndex = i;
                std::cout << "Queue supports both graphics and present\n";
                break;
            }
        }
        if (presentQueueFamilyIndex == queueFamilyProperties.size())
        {
            // there's nothing like a single family index that supports both graphics and present,
            // so look for an other family index that supports present
            for (size_t i = 0; i < queueFamilyProperties.size(); i++)
            {
                if (physicalDevice.getSurfaceSupportKHR(static_cast<uint32_t>(i), surface))
                {
                    std::cout << "Found queue that supports present\n";
                    presentQueueFamilyIndex = i;
                    break;
                }
            }
        }
    }
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

    vk::DeviceCreateInfo deviceInfo;
    deviceInfo.flags = vk::DeviceCreateFlags();
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
    deviceInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();

    return physicalDevice.createDeviceUnique(deviceInfo);
}


