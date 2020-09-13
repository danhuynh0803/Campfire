#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "VulkanContext.h"
#include "VulkanShader.h"

#include <GLFW/glfw3.h>
#include <limits>
#include <vector>
#include <fstream>

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

    const std::vector<const char*> deviceExtensions =
    {
        "VK_KHR_swapchain"
    };

    device =
        physicalDevice.createDeviceUnique(
            vk::DeviceCreateInfo{
                .flags = vk::DeviceCreateFlags(),
                .queueCreateInfoCount = 1,
                .pQueueCreateInfos = &deviceQueueCreateInfo,
                .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
                .ppEnabledExtensionNames = deviceExtensions.data()
            }
        );

    // Get supported formats
    std::vector<vk::SurfaceFormatKHR> formats = physicalDevice.getSurfaceFormatsKHR(surface.get());
    assert(!formats.empty());
    vk::Format format =
        ( formats[0].format == vk::Format::eUndefined ) ? vk::Format::eB8G8R8A8Unorm : formats[0].format;

    vk::SurfaceCapabilitiesKHR surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface.get());
    if (surfaceCapabilities.currentExtent.width == (std::numeric_limits<uint32_t>::max)())
    {
        // If the surface size is undefined, the size is set to the size of the images requested
        // FIXME hardcode for now
        swapChainExtent.width = 1600;
        swapChainExtent.height = 900;
    }
    else
    {
        // if surface size is defined, the swap chain size must match
        swapChainExtent = surfaceCapabilities.currentExtent;
    }

    vk::PresentModeKHR swapchainPresentMode = vk::PresentModeKHR::eFifo;
    std::vector<vk::PresentModeKHR> presentModes = physicalDevice.getSurfacePresentModesKHR(surface.get());
    for (const auto& presentMode : presentModes)
    {
        if (presentMode == vk::PresentModeKHR::eMailbox)
        {
            swapchainPresentMode = vk::PresentModeKHR::eMailbox;
            break;
        }
    }


    vk::SurfaceTransformFlagBitsKHR preTransform =
        (surfaceCapabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity)
        ? vk::SurfaceTransformFlagBitsKHR::eIdentity
        : surfaceCapabilities.currentTransform;

    vk::CompositeAlphaFlagBitsKHR compositeAlpha =
        ( surfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied )
            ? vk::CompositeAlphaFlagBitsKHR::ePreMultiplied
            : ( surfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied )
                ? vk::CompositeAlphaFlagBitsKHR::ePostMultiplied
                : ( surfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eInherit )
                    ? vk::CompositeAlphaFlagBitsKHR::eInherit
                    : vk::CompositeAlphaFlagBitsKHR::eOpaque;

    vk::SwapchainCreateInfoKHR swapChainCreateInfo{
        .flags = vk::SwapchainCreateFlagsKHR(),
        .surface = surface.get(),
        .minImageCount = surfaceCapabilities.minImageCount + 1,
        .imageFormat = format,
        .imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear,
        .imageExtent = swapChainExtent,
        .imageArrayLayers = 1,
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
        .imageSharingMode = vk::SharingMode::eExclusive,
        .preTransform = preTransform,
        .compositeAlpha = compositeAlpha,
        .presentMode = swapchainPresentMode,
        .clipped = true,
        .oldSwapchain = nullptr
    };

    uint32_t queueFamilyIndices[2] = { static_cast<uint32_t>(graphicsQueueFamilyIndex),
                                       static_cast<uint32_t>(presentQueueFamilyIndex) };

    if (graphicsQueueFamilyIndex != presentQueueFamilyIndex)
    {
        swapChainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        swapChainCreateInfo.queueFamilyIndexCount = 2;
        swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }

    // Setup swapchain
    swapChain = device->createSwapchainKHRUnique(swapChainCreateInfo);
    swapChainImages = device->getSwapchainImagesKHR(swapChain.get());
    imageViews.reserve(swapChainImages.size());

    vk::ComponentMapping componentMapping {
        vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA
    };

    vk::ImageSubresourceRange subresourceRange{ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 };
    for (auto image : swapChainImages)
    {
        vk::ImageViewCreateInfo imageViewCreateInfo {
            .flags = vk::ImageViewCreateFlags(),
            .image = image,
            .viewType = vk::ImageViewType::e2D,
            .format = format,
            .components = componentMapping,
            .subresourceRange = subresourceRange
        };
        imageViews.emplace_back( device->createImageViewUnique(imageViewCreateInfo) );
    }

    // Setup command pool
    //commandPool = device->createCommandPoolUnique(
    //    vk::CommandPoolCreateInfo{
    //        .flags = vk::CommandPoolCreateFlags(),
    //        .queueFamilyIndex = static_cast<uint32_t>(graphicsQueueFamilyIndex)
    //    }
    //);

    //commandBuffer =
    //    std::move(
    //        device->allocateCommandBuffersUnique(
    //            vk::CommandBufferAllocateInfo{
    //                .commandPool = commandPool.get(),
    //                .level = vk::CommandBufferLevel::ePrimary,
    //                .commandBufferCount = 1
    //            }
    //        ).front()
    //    );

    CreateGraphicsPipeline();
}

static std::vector<char> readFile(const std::string& filepath)
{
    std::ifstream file(filepath, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("VulkanShader::Failed to open " + filepath);
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

void VulkanContext::CreateGraphicsPipeline()
{
    // Create shader modules
    //VulkanShader vert("../Campfire/Shaders/vert.spv");
    //VulkanShader frag("../Campfire/Shaders/frag.spv");

    std::vector<char> vert = readFile("../Campfire/Shaders/vert.spv");
    std::vector<char> frag = readFile("../Campfire/Shaders/frag.spv");

    vk::ShaderModuleCreateInfo vertexCreateInfo{
        .flags = vk::ShaderModuleCreateFlags(),
        .codeSize = vert.size(),
        .pCode = reinterpret_cast<const uint32_t*>(vert.data())
        //.codeSize = vert.GetBuffer().size(),
        //.pCode = reinterpret_cast<const uint32_t*>(vert.GetBuffer().data())
    };
    vk::UniqueShaderModule vertShaderModule = device->createShaderModuleUnique(vertexCreateInfo);

    vk::ShaderModuleCreateInfo fragCreateInfo{
        .flags = vk::ShaderModuleCreateFlags(),
        .codeSize = frag.size(),
        .pCode = reinterpret_cast<const uint32_t*>(frag.data())
        //.codeSize = frag.GetBuffer().size(),
        //.pCode = reinterpret_cast<const uint32_t*>(frag.GetBuffer().data())
    };
    vk::UniqueShaderModule fragShaderModule = device->createShaderModuleUnique(fragCreateInfo);

    // Create pipeline
    vk::PipelineShaderStageCreateInfo vertShaderStageInfo
    {
        .stage = vk::ShaderStageFlagBits::eVertex,
        .module = vertShaderModule.get(),
        .pName = "main"
    };

    vk::PipelineShaderStageCreateInfo fragShaderStageInfo
    {
        .stage = vk::ShaderStageFlagBits::eFragment,
        .module = fragShaderModule.get(),
        .pName = "main"
    };

    vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    // Setup fixed function part of pipeline

    // Vertex input
    // Binds = spacing btw data and whether data is per-vertex or per-instance
    // Attribute descriptions = type of the attribs passed to vertex shader,
    // including which binding to load them from and at which offset
    vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo {
        .flags = vk::PipelineVertexInputStateCreateFlags(),
        .vertexBindingDescriptionCount = 0,
        .pVertexBindingDescriptions = nullptr,
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions = nullptr
    };

    // Input assembly
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo {
        .flags = vk::PipelineInputAssemblyStateCreateFlags(),
        .topology = vk::PrimitiveTopology::eTriangleList,
        .primitiveRestartEnable = false
    };

    // Setup viewports and scissor rect
    vk::Viewport viewport {
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(swapChainExtent.width),
        .height = static_cast<float>(swapChainExtent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    vk::Rect2D scissors {
        .offset = {0, 0},
        .extent = swapChainExtent
    };

    vk::PipelineViewportStateCreateInfo viewportStateCreateInfo {
        .flags = vk::PipelineViewportStateCreateFlags(),
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissors
    };

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
