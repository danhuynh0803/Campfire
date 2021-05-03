#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanBuffer.h"

#include <limits>
#include <vector>
#include <fstream>

#include <GLFW/glfw3.h>

VulkanContext::VulkanContext(GLFWwindow* window)
    : windowHandle(window)
{
    sVulkanInstance = CreateInstance();
    mDevice = CreateSharedPtr<VulkanDevice>();
    sVulkanContextInstance.reset(this);

    mGraphicsCommandPool = CreateCommandPool(
        static_cast<uint32_t>(GetDevice()->GetQueueFamilyIndex(QueueFamilyType::GRAPHICS))
    );
    mComputeCommandPool = CreateCommandPool(
        static_cast<uint32_t>(GetDevice()->GetQueueFamilyIndex(QueueFamilyType::COMPUTE))
    );

    mSwapChain = CreateSharedPtr<VulkanSwapChain>(window);

    // Creating descriptor pool
    auto swapChainSize = static_cast<uint32_t>(mSwapChain->GetImages().size());
    std::array<vk::DescriptorPoolSize, 3> poolSizes{};
    poolSizes[0].type = vk::DescriptorType::eUniformBuffer;
    poolSizes[0].descriptorCount = swapChainSize;
    poolSizes[1].type = vk::DescriptorType::eCombinedImageSampler;
    poolSizes[1].descriptorCount = swapChainSize;
    poolSizes[2].type = vk::DescriptorType::eStorageImage;
    poolSizes[2].descriptorCount = swapChainSize;

    vk::DescriptorPoolCreateInfo poolInfo;
    // TODO have poolsize be configurable
    poolInfo.maxSets = 10000 * swapChainSize;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    mDescriptorPool = mDevice->GetVulkanDevice().createDescriptorPoolUnique(poolInfo);

    mFrameGraph.CreateRenderFrameGraph();

    // TODO pipeline should be with material?
    mComputePipeline = CreateSharedPtr<VulkanComputePipeline>();

    // These need to be created post-graphics pipeline
    mSwapChain->CreateFramebuffers();
    mSwapChain->CreateBarriers();
}

vk::CommandPool VulkanContext::GetCommandPool(QueueFamilyType type)
{
    switch (type)
    {
        case QueueFamilyType::GRAPHICS:
            return mGraphicsCommandPool.get();
        case QueueFamilyType::COMPUTE:
            return mComputeCommandPool.get();
    }

    CORE_ERROR("Invalid Pipeline type specified for command pool access");
    return {};
}

void VulkanContext::Init()
{
    glfwMakeContextCurrent(windowHandle);
    LOG_INFO("Vulkan initialized");
}

vk::UniqueCommandPool VulkanContext::CreateCommandPool(uint32_t queueFamilyIndex)
{
    return VulkanContext::Get()->GetDevice()->GetVulkanDevice().createCommandPoolUnique(
        vk::CommandPoolCreateInfo {
            vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
            queueFamilyIndex
        }
    );
}

std::vector<vk::UniqueCommandBuffer> VulkanContext::CreateCommandBuffers(uint32_t size)
{
    vk::CommandBufferAllocateInfo commandBufferAllocInfo;
    commandBufferAllocInfo.commandPool = mGraphicsCommandPool.get();
    commandBufferAllocInfo.level = vk::CommandBufferLevel::ePrimary;
    commandBufferAllocInfo.commandBufferCount = size;

    return GetDevice()->GetVulkanDevice().allocateCommandBuffersUnique(commandBufferAllocInfo);
}


void VulkanContext::RecreateSwapChain()
{
    GetDevice()->GetVulkanDevice().waitIdle();

    mSwapChain.reset();

    mSwapChain = CreateSharedPtr<VulkanSwapChain>(windowHandle);

    //mGraphicsPipeline->RecreatePipeline();

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
    vk::ApplicationInfo appInfo;
    appInfo.pApplicationName = "Vulkan Context";
    appInfo.applicationVersion = 1;
    appInfo.pEngineName = "Campfire Engine";
    appInfo.engineVersion = 1;
    appInfo.apiVersion = VK_API_VERSION_1_2;

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

    vk::InstanceCreateInfo instanceCreateInfo;
    instanceCreateInfo.pApplicationInfo = &appInfo;

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
