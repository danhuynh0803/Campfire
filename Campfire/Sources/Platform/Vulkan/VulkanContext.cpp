#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "VulkanContext.h"
#include "VulkanBuffer.h"

#include <limits>
#include <vector>
#include <fstream>

VulkanContext::VulkanContext(GLFWwindow* window)
    : windowHandle(window)
{
    sVulkanInstance = CreateInstance();
    mSurface = CreateSurfaceKHR(window);
    mDevice = CreateSharedPtr<VulkanDevice>();
    sVulkanContextInstance.reset(this);
    mSwapChain = CreateSharedPtr<VulkanSwapChain>();
    mGraphicsPipeline = CreateSharedPtr<VulkanPipeline>(PipelineType::GRAPHICS);

    // These need to be created post-graphics pipeline
    mSwapChain->CreateFramebuffers();
    mSwapChain->CreateBarriers();
}

void VulkanContext::Init()
{
    glfwMakeContextCurrent(windowHandle);
    LOG_INFO("Vulkan initialized");
}

void VulkanContext::SwapBuffers()
{
    mSwapChain->Present();
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

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

    // Enable validation layers
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    if (enableValidationLayers && !CheckValidationLayerSupport(validationLayers))
    {
        throw std::runtime_error("Validation layers enabled, but not available");
    }

    vk::InstanceCreateInfo instanceCreateInfo {
        .pApplicationInfo = &appInfo
    };

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    instanceCreateInfo.enabledExtensionCount = glfwExtensionCount;
    instanceCreateInfo.ppEnabledExtensionNames = glfwExtensions;

    if (enableValidationLayers)
    {
        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        instanceCreateInfo.enabledLayerCount = 0;
    }

    return vk::createInstanceUnique(instanceCreateInfo);
}

bool VulkanContext::CheckValidationLayerSupport(const std::vector<const char*>& validationLayers)
{
    std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();
    for (const char* layerName : validationLayers)
    {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            return false;
        }
    }

    return true;
}

vk::UniqueSurfaceKHR VulkanContext::CreateSurfaceKHR(GLFWwindow* window)
{
    VkSurfaceKHR surfaceTmp;
    if (glfwCreateWindowSurface(VkInstance(sVulkanInstance.get()), window, nullptr, &surfaceTmp) != VK_SUCCESS)
    {
        LOG_ERROR("Could not init window");
    }
    vk::ObjectDestroy<vk::Instance, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE> _deleter(sVulkanInstance.get());
    return vk::UniqueSurfaceKHR(vk::SurfaceKHR(surfaceTmp), _deleter);
}
