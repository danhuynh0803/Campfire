#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "VulkanContext.h"
#include <GLFW/glfw3.h>

VulkanContext::VulkanContext()
{
    instance = CreateInstance();
    physicalDevice = GetPhysicalDevice();

    // Determine if device contains a graphics queue
    queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

    size_t graphicsQueueFamilyIndex = std::distance (
            queueFamilyProperties.begin(),
            std::find_if (
                queueFamilyProperties.begin(), queueFamilyProperties.end(), []( vk::QueueFamilyProperties const& qfp) {
                    return qfp.queueFlags & vk::QueueFlagBits::eGraphics;
                }
            )
        );

    assert( graphicsQueueFamilyIndex < queueFamilyProperties.size() );

    float queuePriority = 0.0f;
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo{
        .flags = vk::DeviceQueueCreateFlags(),
        .queueFamilyIndex = static_cast<uint32_t>(graphicsQueueFamilyIndex),
        .queueCount = 1,
        .pQueuePriorities = &queuePriority
    };

    vk::UniqueDevice device =
        physicalDevice.createDeviceUnique(
            vk::DeviceCreateInfo{
                .flags = vk::DeviceCreateFlags(),
                .pQueueCreateInfos = &deviceQueueCreateInfo
            }
        );
}

VulkanContext::~VulkanContext()
{
    instance.destroy();
}

vk::Instance VulkanContext::CreateInstance()
{
    vk::ApplicationInfo appInfo {
        .pApplicationName = "Vulkan Context",
        .applicationVersion = 1,
        .pEngineName = "Campfire Engine",
        .engineVersion = 1,
        .apiVersion = VK_API_VERSION_1_2
    };

    vk::InstanceCreateInfo instanceCreateInfo {
        .pApplicationInfo = &appInfo
    };

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    instanceCreateInfo.enabledExtensionCount = glfwExtensionCount;
    instanceCreateInfo.ppEnabledExtensionNames = glfwExtensions;

    instanceCreateInfo.enabledLayerCount = 0;

    return vk::createInstance(instanceCreateInfo);
}

vk::PhysicalDevice VulkanContext::GetPhysicalDevice()
{
    physicalDevices = instance.enumeratePhysicalDevices();
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

bool VulkanContext::IsDeviceSuitable(vk::PhysicalDevice device)
{
    vk::PhysicalDeviceProperties deviceProperties = device.getProperties();
    vk::PhysicalDeviceFeatures deviceFeatures = device.getFeatures();

    // TODO add various feature checks
    return true;
}
