#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "VulkanContext.h"
#include <GLFW/glfw3.h>

VulkanContext::VulkanContext(GLFWwindow* window)
    : windowHandle(window)
{
    instance = CreateInstance();

    // Setup VkSurfaceKHR
    {
        VkSurfaceKHR surfaceTmp;
        if (glfwCreateWindowSurface(VkInstance(instance.get()), window, nullptr, &surfaceTmp) != VK_SUCCESS)
        {
            std::cout << "Could not init window\n";
        }
        vk::ObjectDestroy<vk::Instance, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE> _deleter(instance.get());
        surface = vk::UniqueSurfaceKHR(vk::SurfaceKHR(surfaceTmp), _deleter);
    }

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

    // Get queueFamilyIndex that supports present
    // First check if graphicsQueueFamilyIndex is good enough
    size_t presentQueueFamilyIndex =
        physicalDevice.getSurfaceSupportKHR( static_cast<uint32_t>(graphicsQueueFamilyIndex), surface.get() )
            ? graphicsQueueFamilyIndex
            : queueFamilyProperties.size();

    if (presentQueueFamilyIndex == queueFamilyProperties.size())
    {
        // the graphicsQueueFamilyIndex doesn't support present -> look for an other family index that supports both
        // graphics and present
        for (size_t i = 0; i < queueFamilyProperties.size(); i++)
        {
            if ((queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) &&
                physicalDevice.getSurfaceSupportKHR(static_cast<uint32_t>(i), surface.get()))
            {
                //graphicsQueueFamilyIndex = vk::su::checked_cast<uint32_t>(i);
                graphicsQueueFamilyIndex = i;
                presentQueueFamilyIndex = i;
                std::cout << "Queue supports both graphics and present\n";
                break;
            }
        }
        if (presentQueueFamilyIndex == queueFamilyProperties.size())
        {
            // there's nothing like a single family index that supports both graphics and present -> look for an other
            // family index that supports present
            for (size_t i = 0; i < queueFamilyProperties.size(); i++)
            {
                if (physicalDevice.getSurfaceSupportKHR(static_cast<uint32_t>(i), surface.get()))
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
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo{
        .flags = vk::DeviceQueueCreateFlags(),
        .queueFamilyIndex = static_cast<uint32_t>(graphicsQueueFamilyIndex),
        .queueCount = 1,
        .pQueuePriorities = &queuePriority
    };

    device =
        physicalDevice.createDeviceUnique(
            vk::DeviceCreateInfo{
                .flags = vk::DeviceCreateFlags(),
                .pQueueCreateInfos = &deviceQueueCreateInfo
            }
        );

    commandPool = device->createCommandPoolUnique(
        vk::CommandPoolCreateInfo {
            .flags = vk::CommandPoolCreateFlags(),
            .queueFamilyIndex = static_cast<uint32_t>(graphicsQueueFamilyIndex)
        }
    );

    commandBuffer =
        std::move(
            device->allocateCommandBuffersUnique(
                vk::CommandBufferAllocateInfo{
                    .commandPool = commandPool.get(),
                    .level = vk::CommandBufferLevel::ePrimary,
                    .commandBufferCount = 1
                }
            ).front()
        );

}

VulkanContext::~VulkanContext()
{
    instance->destroy();
}

void VulkanContext::Init()
{
    glfwMakeContextCurrent(windowHandle);
    LOG_INFO("Vulkan initialized");
}

void VulkanContext::SwapBuffers()
{
    // TODO
}

vk::UniqueInstance VulkanContext::CreateInstance()
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

    return vk::createInstanceUnique(instanceCreateInfo);
}

vk::PhysicalDevice VulkanContext::GetPhysicalDevice()
{
    physicalDevices = instance->enumeratePhysicalDevices();
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
