#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "VulkanContext.h"
#include "VulkanBuffer.h"
//#include "VulkanShader.h"

#include <GLFW/glfw3.h>
#include <limits>
#include <vector>
#include <fstream>

vk::UniqueDevice VulkanContext::device;

static size_t graphicsQueueFamilyIndex;
static size_t presentQueueFamilyIndex;
static const int MAX_FRAMES_IN_FLIGHT = 2;

VulkanContext::VulkanContext(GLFWwindow* window)
    : windowHandle(window)
{
    instance = CreateInstance();

    surface = CreateSurfaceKHR(window);

    physicalDevice = GetPhysicalDevice();

    device = CreateLogicalDevice();

    graphicsQueue = device->getQueue(graphicsQueueFamilyIndex, 0);
    presentQueue = device->getQueue(presentQueueFamilyIndex, 0);

    SetupSwapChain();

    graphicsPipeline = CreateGraphicsPipeline();

    CreateFramebuffers();

    commandPool = CreateCommandPool(static_cast<uint32_t>(graphicsQueueFamilyIndex));

    commandBuffers = CreateCommandBuffers(static_cast<uint32_t>(swapChainFramebuffers.size()));

    // Start recording command buffers
    for (size_t i = 0; i < commandBuffers.size(); ++i)
    {
        vk::CommandBufferBeginInfo beginInfo
        {
            .flags = vk::CommandBufferUsageFlags()
            , .pInheritanceInfo = nullptr
        };

        commandBuffers[i]->begin(beginInfo);

        // Start render pass
        vk::ClearValue clearValues;
        clearValues.color = vk::ClearColorValue(std::array<float, 4>({ { 0.0f, 0.0f, 0.0f, 1.0f } }));
        // TODO
        //clearValues.depthStencil = vk::ClearDepthStencilValue{ 1.0f, 0 };

        vk::Rect2D renderArea
        {
            .offset = {0, 0}
            , .extent = swapChainExtent
        };

        vk::RenderPassBeginInfo renderPassBeginInfo
        {
            .renderPass = renderPass.get()
            , .framebuffer = swapChainFramebuffers[i].get()
            , .renderArea = renderArea
            , .clearValueCount = 1
            , .pClearValues = &clearValues
        };

        commandBuffers[i]->beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
        commandBuffers[i]->bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline.get());
        commandBuffers[i]->draw(3, 1, 0, 0);
        commandBuffers[i]->endRenderPass();
        commandBuffers[i]->end();
    }

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

    return device->createSemaphoreUnique(semaphoreCreateInfo);
}

vk::UniqueFence VulkanContext::CreateFence()
{
    vk::FenceCreateInfo fenceCreateInfo
    {
        .flags = vk::FenceCreateFlagBits::eSignaled
    };

    return device->createFenceUnique(fenceCreateInfo);
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
    // move later to renderer, but just put
    // draw calls in here for now for quick testing
    static size_t currentFrame = 0;

    device->waitForFences(inFlightFences[currentFrame].get(), VK_TRUE, UINT64_MAX);

    auto& imageAvailableSemaphore = imageAvailableSemaphores[currentFrame];
    uint32_t imageIndex = device->acquireNextImageKHR(swapChain.get(), (std::numeric_limits<uint64_t>::max)(), imageAvailableSemaphore.get(), {});

    // Check if previous frame is using this image (wait on its fence)
    if (imagesInFlight[imageIndex])
    {
      //device->waitForFences(imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
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
    if (glfwCreateWindowSurface(VkInstance(instance.get()), window, nullptr, &surfaceTmp) != VK_SUCCESS)
    {
        std::cout << "Could not init window\n";
    }
    vk::ObjectDestroy<vk::Instance, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE> _deleter(instance.get());
    return vk::UniqueSurfaceKHR(vk::SurfaceKHR(surfaceTmp), _deleter);
}

vk::UniqueDevice VulkanContext::CreateLogicalDevice()
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

    // Get queueFamilyIndex that supports present
    // First check if graphicsQueueFamilyIndex is good enough
     presentQueueFamilyIndex =
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

    return physicalDevice.createDeviceUnique(
            vk::DeviceCreateInfo
            {
                .flags = vk::DeviceCreateFlags(),
                .queueCreateInfoCount = 1,
                .pQueueCreateInfos = &deviceQueueCreateInfo,
                .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
                .ppEnabledExtensionNames = deviceExtensions.data()
            }
        );
}

void VulkanContext::SetupSwapChain()
{
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
            .format = swapChainImageFormat,
            .components = componentMapping,
            .subresourceRange = subresourceRange
        };
        imageViews.emplace_back( device->createImageViewUnique(imageViewCreateInfo) );
    }
}

vk::UniqueCommandPool VulkanContext::CreateCommandPool(uint32_t queueFamilyIndex)
{
    return device->createCommandPoolUnique(
        vk::CommandPoolCreateInfo{
            .flags = vk::CommandPoolCreateFlags(),
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

    return device->allocateCommandBuffersUnique(commandBufferAllocInfo);
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

vk::UniquePipeline VulkanContext::CreateGraphicsPipeline()
{
    // Create shader modules
    //VulkanShader vert("../Campfire/Shaders/vert.spv");
    //VulkanShader frag("../Campfire/Shaders/frag.spv");

    std::vector<char> vert = readFile("../Campfire/Shaders/vert.spv");
    std::vector<char> frag = readFile("../Campfire/Shaders/frag.spv");

    vk::ShaderModuleCreateInfo vertexCreateInfo
    {
        .flags = vk::ShaderModuleCreateFlags(),
        .codeSize = vert.size(),
        .pCode = reinterpret_cast<const uint32_t*>(vert.data())
        //.codeSize = vert.GetBuffer().size(),
        //.pCode = reinterpret_cast<const uint32_t*>(vert.GetBuffer().data())
    };
    vk::UniqueShaderModule vertShaderModule = device->createShaderModuleUnique(vertexCreateInfo);

    vk::ShaderModuleCreateInfo fragCreateInfo
    {
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

    auto bindingDescription = Vertex::GetBindingDescription();
    auto attributeDescriptions = Vertex::GetAttributeDescriptions();

    vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo
    {
        .flags = vk::PipelineVertexInputStateCreateFlags()
        , .vertexBindingDescriptionCount = 1
        , .pVertexBindingDescriptions = &bindingDescription
        , .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size())
        , .pVertexAttributeDescriptions = attributeDescriptions.data()
    };

    // Input assembly
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo
    {
        .flags = vk::PipelineInputAssemblyStateCreateFlags(),
        .topology = vk::PrimitiveTopology::eTriangleList,
        .primitiveRestartEnable = VK_FALSE
    };

    // Setup viewports and scissor rect
    vk::Viewport viewport
    {
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(swapChainExtent.width),
        .height = static_cast<float>(swapChainExtent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    vk::Rect2D scissors
    {
        .offset = {0, 0},
        .extent = swapChainExtent
    };

    vk::PipelineViewportStateCreateInfo viewportStateCreateInfo
    {
        .flags = vk::PipelineViewportStateCreateFlags(),
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissors
    };

    // Setup rasterizer
    // Note: depthClampEnable: if true, fragments beyond
    // near and far planes will be clamped instead of being discard
    vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo
    {
        .flags = vk::PipelineRasterizationStateCreateFlags()
        , .depthClampEnable = VK_FALSE
        , .rasterizerDiscardEnable = VK_FALSE
        , .polygonMode = vk::PolygonMode::eFill
        , .cullMode = vk::CullModeFlagBits::eBack
        , .frontFace = vk::FrontFace::eClockwise
        , .depthBiasEnable = VK_FALSE
        , .depthBiasConstantFactor = 0.0f
        , .depthBiasClamp = 0.0f
        , .depthBiasSlopeFactor = 0.0f
        , .lineWidth = 1.0f
    };

    // Enable multisampling
    vk::PipelineMultisampleStateCreateInfo multisampleCreateInfo
    {
        .flags = vk::PipelineMultisampleStateCreateFlags()
        , .rasterizationSamples = vk::SampleCountFlagBits::e1
        , .sampleShadingEnable = VK_FALSE
        , .minSampleShading = 1.0f
        , .pSampleMask = nullptr
        , .alphaToCoverageEnable = VK_FALSE
        , .alphaToOneEnable = VK_FALSE
    };

    // TODO Setup depth and stencil testing
    vk::PipelineDepthStencilStateCreateInfo depthStencilCreateInfo
    {
    };

    // Setup color blending
    vk::PipelineColorBlendAttachmentState colorBlendAttachment
    {
        .blendEnable = VK_FALSE
        , .srcColorBlendFactor = vk::BlendFactor::eOne
        , .dstColorBlendFactor = vk::BlendFactor::eZero
        , .colorBlendOp = vk::BlendOp::eAdd
        , .srcAlphaBlendFactor = vk::BlendFactor::eOne
        , .dstAlphaBlendFactor = vk::BlendFactor::eZero
        , .alphaBlendOp = vk::BlendOp::eAdd
        , .colorWriteMask =
            vk::ColorComponentFlagBits::eR
            | vk::ColorComponentFlagBits::eG
            | vk::ColorComponentFlagBits::eB
            | vk::ColorComponentFlagBits::eA
    };

    vk::PipelineColorBlendStateCreateInfo colorBlendCreateInfo
    {
        .flags = vk::PipelineColorBlendStateCreateFlags()
        , .logicOpEnable = VK_FALSE
        , .logicOp = vk::LogicOp::eCopy
        , .attachmentCount = 1
        , .pAttachments = &colorBlendAttachment
    };

    // Setup dynamic state - these can be changed without recreating the pipeline
    std::vector<vk::DynamicState> dynamicStates =
    {
        vk::DynamicState::eViewport
        , vk::DynamicState::eLineWidth
    };

    vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo
    {
        .flags = vk::PipelineDynamicStateCreateFlags()
        , .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size())
        , .pDynamicStates = dynamicStates.data()
    };

    // Setup pipeline layout
    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo
    {
        .setLayoutCount = 0
        , .pSetLayouts = nullptr
        , .pushConstantRangeCount = 0
        , .pPushConstantRanges = nullptr
    };
    pipelineLayout = device->createPipelineLayoutUnique(pipelineLayoutCreateInfo);

    // ----------- Setup Renderpasses ---------------------

    // Setup loadOp and StoreOp which determines what to do
    // with the data in the attachment before rendering and
    // after rendering
    vk::AttachmentDescription colorAttachment
    {
        .format = swapChainImageFormat
        , .samples = vk::SampleCountFlagBits::e1
        , .loadOp = vk::AttachmentLoadOp::eClear // Clear the values to a constant at start
        , .storeOp = vk::AttachmentStoreOp::eStore // Rendered contents store in memory and can be read later
        , .stencilLoadOp = vk::AttachmentLoadOp::eDontCare
        , .stencilStoreOp = vk::AttachmentStoreOp::eDontCare
        , .initialLayout = vk::ImageLayout::eUndefined
        , .finalLayout = vk::ImageLayout::ePresentSrcKHR
    };

    // Setup subpasses
    vk::AttachmentReference colorAttachmentRef
    {
        .attachment = 0
        , .layout = vk::ImageLayout::eColorAttachmentOptimal
    };

    vk::SubpassDescription subpass
    {
        .pipelineBindPoint = vk::PipelineBindPoint::eGraphics
        , .colorAttachmentCount = 1
        , .pColorAttachments = &colorAttachmentRef
    };

    // Setup subpass dependencies
    vk::SubpassDependency subpassDependency
    {
        .srcSubpass = VK_SUBPASS_EXTERNAL
        , .dstSubpass = 0
        , .srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput
        , .dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput
        , .srcAccessMask = static_cast<vk::AccessFlagBits>(0)
        , .dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite
    };

    vk::RenderPassCreateInfo renderPassCreateInfo
    {
        .flags = vk::RenderPassCreateFlags()
        , .attachmentCount = 1
        , .pAttachments = &colorAttachment
        , .subpassCount = 1
        , .pSubpasses = &subpass
        , .dependencyCount = 1
        , .pDependencies = &subpassDependency
    };
    renderPass = device->createRenderPassUnique(renderPassCreateInfo);

    // Create Graphics pipeline
    vk::GraphicsPipelineCreateInfo pipelineCreateInfo
    {
        .flags = vk::PipelineCreateFlags()
        , .stageCount = 2
        , .pStages = shaderStages
        , .pVertexInputState = &vertexInputStateCreateInfo
        , .pInputAssemblyState = &inputAssemblyStateCreateInfo
        , .pViewportState = &viewportStateCreateInfo
        , .pRasterizationState = &rasterizationStateCreateInfo
        , .pMultisampleState = &multisampleCreateInfo
        , .pDepthStencilState = nullptr
        , .pColorBlendState = &colorBlendCreateInfo
        , .pDynamicState = nullptr
        , .layout = pipelineLayout.get()
        , .renderPass = renderPass.get()
        , .subpass = 0
        , .basePipelineHandle = nullptr
        , .basePipelineIndex = -1
    };

    return device->createGraphicsPipelineUnique(nullptr, pipelineCreateInfo);
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
            , .renderPass = renderPass.get()
            , .attachmentCount = 1
            , .pAttachments = attachments
            , .width = swapChainExtent.width
            , .height = swapChainExtent.height
            , .layers = 1
        };

        swapChainFramebuffers[i] = device->createFramebufferUnique(framebufferCreateInfo);
    }
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
