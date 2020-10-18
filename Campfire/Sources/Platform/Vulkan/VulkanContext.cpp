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
    mDevice = CreateSharedPtr<VulkanDevice>();
    sVulkanContextInstance.reset(this);
    mSwapChain = CreateSharedPtr<VulkanSwapChain>(window);
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

void VulkanContext::RecreateSwapChain()
{
    GetDevice()->GetVulkanDevice().waitIdle();

    mSwapChain.reset();

    mSwapChain = CreateSharedPtr<VulkanSwapChain>(windowHandle);

    mGraphicsPipeline->RecreatePipeline();

    // These need to be created post-graphics pipeline
    mSwapChain->CreateFramebuffers();
    mSwapChain->CreateBarriers();
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
