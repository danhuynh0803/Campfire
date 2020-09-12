#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "VulkanContext.h"
#include <GLFW/glfw3.h>

VulkanContext::VulkanContext()
{
    instance = CreateInstance();
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
