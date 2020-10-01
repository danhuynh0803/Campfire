#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "VulkanContext.h"
#include "VulkanBuffer.h"

#include <limits>
#include <vector>
#include <fstream>

static const int MAX_FRAMES_IN_FLIGHT = 2;

// TODO move to renderer
void VulkanContext::DrawIndexed(vk::Buffer vertexBuffer, vk::Buffer indexBuffer, uint32_t count)
{
    auto device = mDevice->GetVulkanDevice();
    device.waitForFences(inFlightFences[currentFrame].get(), VK_TRUE, UINT64_MAX);

    auto& imageAvailableSemaphore = imageAvailableSemaphores[currentFrame];
    uint32_t imageIndex = device.acquireNextImageKHR(swapChain.get(), (std::numeric_limits<uint64_t>::max)(), imageAvailableSemaphore.get(), {});

    // Check if previous frame is using this image (wait on its fence)
    if (imagesInFlight[imageIndex])
    {
        device.waitForFences(imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }
    // Mark the image as now being in use by this frame
    imagesInFlight[imageIndex] = inFlightFences[currentFrame].get();

    vk::Semaphore waitSemaphores[] = { imageAvailableSemaphore.get() };
    // wait to write colors to the image until it's available
    // Note: each entry in waitStages will correspond to the same semaphore in waitSemaphores
    vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

    vk::SubmitInfo submitInfo
    {
        .waitSemaphoreCount = 1
        , .pWaitSemaphores = waitSemaphores
        , .pWaitDstStageMask = waitStages
        , .commandBufferCount = 1
        , .pCommandBuffers = &commandBuffers[imageIndex].get()
    };

    auto& renderFinishedSemaphore = renderFinishedSemaphores[currentFrame];
    vk::Semaphore signalSemaphores[] = { renderFinishedSemaphore.get() };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    device.resetFences(inFlightFences[currentFrame].get());

    // Start recording command buffers
    vk::CommandBufferBeginInfo beginInfo
    {
        .flags = vk::CommandBufferUsageFlags()
        , .pInheritanceInfo = nullptr
    };

    commandBuffers[imageIndex]->begin(beginInfo);

    // Start render pass
    vk::ClearValue clearValues;
    clearValues.color = vk::ClearColorValue(std::array<float, 4>({ { 0.0f, 0.0f, 0.0f, 1.0f } }));
    //clearValues.depthStencil = vk::ClearDepthStencilValue{ 1.0f, 0 };

    vk::Rect2D renderArea
    {
        .offset = {0, 0}
        , .extent = swapChainExtent
    };

    vk::RenderPassBeginInfo renderPassBeginInfo
    {
        .renderPass = mGraphicsPipeline->GetVulkanRenderPass()
        , .framebuffer = swapChainFramebuffers[imageIndex].get()
        , .renderArea = renderArea
        , .clearValueCount = 1
        , .pClearValues = &clearValues
    };

    commandBuffers[imageIndex]->beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

    commandBuffers[imageIndex]->bindPipeline(vk::PipelineBindPoint::eGraphics, mGraphicsPipeline->GetVulkanPipeline());

    vk::Buffer vertexBuffers[] = { vertexBuffer };
    vk::DeviceSize offsets[] = { 0 };
    commandBuffers[imageIndex]->bindVertexBuffers(0, 1, vertexBuffers, offsets);

    auto& descriptorSets = mGraphicsPipeline->descriptorSets;
    commandBuffers[imageIndex]->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mGraphicsPipeline->GetVulkanPipelineLayout(), 0, 1, &descriptorSets[imageIndex].get(), 0, nullptr);

    commandBuffers[imageIndex]->bindIndexBuffer(indexBuffer, 0, vk::IndexType::eUint32);

    commandBuffers[imageIndex]->drawIndexed(count, 1, 0, 0, 0);

    commandBuffers[imageIndex]->endRenderPass();

    commandBuffers[imageIndex]->end();

    auto graphicsQueue = mDevice->GetGraphicsQueue();
    graphicsQueue.submit(submitInfo, inFlightFences[currentFrame].get());

    vk::PresentInfoKHR presentInfo
    {
        .waitSemaphoreCount = 1
        , .pWaitSemaphores = signalSemaphores
    };

    vk::SwapchainKHR swapChains[] = { swapChain.get() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    auto presentQueue = mDevice->GetPresentQueue();
    presentQueue.presentKHR(&presentInfo);

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

VulkanContext::VulkanContext(GLFWwindow* window)
    : windowHandle(window)
{
    sVulkanInstance = CreateInstance();

    surface = CreateSurfaceKHR(window);

    mDevice = CreateSharedPtr<VulkanDevice>();

    sVulkanContextInstance.reset(this);

    SetupSwapChain();

    mGraphicsPipeline = CreateSharedPtr<VulkanPipeline>(PipelineType::GRAPHICS);

    CreateFramebuffers();

    commandPool = CreateCommandPool(static_cast<uint32_t>(mDevice->GetQueueFamilyIndex(QueueFamilyType::GRAPHICS)));

    commandBuffers = CreateCommandBuffers(static_cast<uint32_t>(swapChainFramebuffers.size()));

    // TODO refactor into separate swapchain class
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        imageAvailableSemaphores.emplace_back(CreateSemaphore());
        renderFinishedSemaphores.emplace_back(CreateSemaphore());
        inFlightFences.emplace_back(CreateFence());
    }

    imagesInFlight.resize(swapChainImages.size());
}

vk::UniqueSemaphore VulkanContext::CreateSemaphore()
{
    // Create semaphores for signaling when an image is ready to render
    // and when an image is done rendering
    vk::SemaphoreCreateInfo semaphoreCreateInfo
    {
        .flags = vk::SemaphoreCreateFlags()
    };

    return mDevice->GetVulkanDevice().createSemaphoreUnique(semaphoreCreateInfo);
}

vk::UniqueFence VulkanContext::CreateFence()
{
    vk::FenceCreateInfo fenceCreateInfo
    {
        .flags = vk::FenceCreateFlagBits::eSignaled
    };

    return mDevice->GetVulkanDevice().createFenceUnique(fenceCreateInfo);
}

VulkanContext::~VulkanContext()
{
    sVulkanInstance->destroy();
}

void VulkanContext::Init()
{
    glfwMakeContextCurrent(windowHandle);
    LOG_INFO("Vulkan initialized");
}

void VulkanContext::SwapBuffers()
{
/*
    device->waitForFences(inFlightFences[currentFrame].get(), VK_TRUE, UINT64_MAX);

    auto& imageAvailableSemaphore = imageAvailableSemaphores[currentFrame];
    uint32_t imageIndex = device->acquireNextImageKHR(swapChain.get(), (std::numeric_limits<uint64_t>::max)(), imageAvailableSemaphore.get(), {});

    // Check if previous frame is using this image (wait on its fence)
    if (imagesInFlight[imageIndex])
    {
        device->waitForFences(imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }
    // Mark the image as now being in use by this frame
    imagesInFlight[imageIndex] = inFlightFences[currentFrame].get();

    vk::Semaphore waitSemaphores[] = { imageAvailableSemaphore.get() };
    // wait to write colors to the image until it's available
    // Note: each entry in waitStages will correspond to the same semaphore in waitSemaphores
    vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

    vk::SubmitInfo submitInfo
    {
        .waitSemaphoreCount = 1
        , .pWaitSemaphores = waitSemaphores
        , .pWaitDstStageMask = waitStages
        , .commandBufferCount = 1
        , .pCommandBuffers = &commandBuffers[imageIndex].get()
    };

    auto& renderFinishedSemaphore = renderFinishedSemaphores[currentFrame];
    vk::Semaphore signalSemaphores[] = { renderFinishedSemaphore.get() };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    device->resetFences(inFlightFences[currentFrame].get());

    graphicsQueue.submit(submitInfo, inFlightFences[currentFrame].get());

    vk::PresentInfoKHR presentInfo
    {
        .waitSemaphoreCount = 1
        , .pWaitSemaphores = signalSemaphores
    };

    vk::SwapchainKHR swapChains[] = { swapChain.get() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    presentQueue.presentKHR(&presentInfo);

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
*/
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
        std::cout << "Could not init window\n";
    }
    vk::ObjectDestroy<vk::Instance, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE> _deleter(sVulkanInstance.get());
    return vk::UniqueSurfaceKHR(vk::SurfaceKHR(surfaceTmp), _deleter);
}

void VulkanContext::SetupSwapChain()
{
    auto physicalDevice = mDevice->GetVulkanPhysicalDevice();
    auto device = mDevice->GetVulkanDevice();

    // Get supported formats
    std::vector<vk::SurfaceFormatKHR> formats = physicalDevice.getSurfaceFormatsKHR(surface.get());
    assert(!formats.empty());
    swapChainImageFormat =
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
        .imageFormat = swapChainImageFormat,
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

    auto graphicsQueueFamilyIndex = mDevice->GetQueueFamilyIndex(QueueFamilyType::GRAPHICS);
    auto presentQueueFamilyIndex = mDevice->GetQueueFamilyIndex(QueueFamilyType::PRESENT);

    uint32_t queueFamilyIndices[2] = { static_cast<uint32_t>(graphicsQueueFamilyIndex),
                                       static_cast<uint32_t>(presentQueueFamilyIndex) };

    if (graphicsQueueFamilyIndex != presentQueueFamilyIndex)
    {
        swapChainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        swapChainCreateInfo.queueFamilyIndexCount = 2;
        swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }

    // Setup swapchain
    swapChain = device.createSwapchainKHRUnique(swapChainCreateInfo);
    swapChainImages = device.getSwapchainImagesKHR(swapChain.get());
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
            .format = swapChainImageFormat,
            .components = componentMapping,
            .subresourceRange = subresourceRange
        };
        imageViews.emplace_back( device.createImageViewUnique(imageViewCreateInfo) );
    }
}

vk::UniqueCommandPool VulkanContext::CreateCommandPool(uint32_t queueFamilyIndex)
{
    return mDevice->GetVulkanDevice().createCommandPoolUnique(
        vk::CommandPoolCreateInfo {
            .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
            .queueFamilyIndex = queueFamilyIndex
        }
    );
}

std::vector<vk::UniqueCommandBuffer> VulkanContext::CreateCommandBuffers(uint32_t size)
{
    vk::CommandBufferAllocateInfo commandBufferAllocInfo
    {
        .commandPool = commandPool.get()
        , .level = vk::CommandBufferLevel::ePrimary
        , .commandBufferCount = size
    };

    return mDevice->GetVulkanDevice().allocateCommandBuffersUnique(commandBufferAllocInfo);
}

void VulkanContext::CreateFramebuffers()
{
    swapChainFramebuffers.resize(imageViews.size());

    for (size_t i = 0; i < imageViews.size(); ++i)
    {
        vk::ImageView attachments[] =
        {
            imageViews[i].get()
        };

        vk::FramebufferCreateInfo framebufferCreateInfo
        {
            .flags = vk::FramebufferCreateFlags()
            , .renderPass = mGraphicsPipeline->GetVulkanRenderPass()
            , .attachmentCount = 1
            , .pAttachments = attachments
            , .width = swapChainExtent.width
            , .height = swapChainExtent.height
            , .layers = 1
        };

        swapChainFramebuffers[i] = mDevice->GetVulkanDevice().createFramebufferUnique(framebufferCreateInfo);
    }
}
