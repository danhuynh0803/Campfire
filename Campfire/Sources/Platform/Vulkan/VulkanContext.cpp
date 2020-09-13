#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "VulkanContext.h"
#include <GLFW/glfw3.h>
#include <limits>

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
    vk::Extent2D swapchainExtent;
    if (surfaceCapabilities.currentExtent.width == (std::numeric_limits<uint32_t>::max)())
    {
        // If the surface size is undefined, the size is set to the size of the images requested
        // FIXME hardcode for now
        swapchainExtent.width = 1600;
        swapchainExtent.height = 900;
    }
    else
    {
        // if surface size is defined, the swap chain size must match
        swapchainExtent = surfaceCapabilities.currentExtent;
    }

    vk::PresentModeKHR swapchainPresentMode = vk::PresentModeKHR::eFifo;

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
        .minImageCount = surfaceCapabilities.minImageCount,
        .imageFormat = format,
        .imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear,
        .imageExtent = swapchainExtent,
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
    std::vector<vk::Image> swapChainImages = device->getSwapchainImagesKHR(swapChain.get());
    std::vector<vk::UniqueImageView> imageViews;
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
