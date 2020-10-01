#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "VulkanSwapChain.h"
#include "VulkanContext.h"

VulkanSwapChain::VulkanSwapChain()
{
    auto devicePtr = VulkanContext::Get()->GetDevice();
    auto physicalDevice = devicePtr->GetVulkanPhysicalDevice();
    auto surface = VulkanContext::Get()->GetSurface();

    // Get supported formats
    std::vector<vk::SurfaceFormatKHR> formats = physicalDevice.getSurfaceFormatsKHR(surface);
    assert(!formats.empty());
    swapChainImageFormat =
        (formats[0].format == vk::Format::eUndefined) ? vk::Format::eB8G8R8A8Unorm : formats[0].format;

    vk::SurfaceCapabilitiesKHR surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
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
    std::vector<vk::PresentModeKHR> presentModes = physicalDevice.getSurfacePresentModesKHR(surface);
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
        (surfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied)
        ? vk::CompositeAlphaFlagBitsKHR::ePreMultiplied
        : (surfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied)
            ? vk::CompositeAlphaFlagBitsKHR::ePostMultiplied
            : (surfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eInherit)
                ? vk::CompositeAlphaFlagBitsKHR::eInherit
                : vk::CompositeAlphaFlagBitsKHR::eOpaque;

    vk::SwapchainCreateInfoKHR swapChainCreateInfo;
    swapChainCreateInfo.flags = vk::SwapchainCreateFlagsKHR();
    swapChainCreateInfo.surface = surface;
    swapChainCreateInfo.minImageCount = surfaceCapabilities.minImageCount + 1;
    swapChainCreateInfo.imageFormat = swapChainImageFormat;
    swapChainCreateInfo.imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
    swapChainCreateInfo.imageExtent = swapChainExtent;
    swapChainCreateInfo.imageArrayLayers = 1;
    swapChainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
    swapChainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
    swapChainCreateInfo.preTransform = preTransform;
    swapChainCreateInfo.compositeAlpha = compositeAlpha;
    swapChainCreateInfo.presentMode = swapchainPresentMode;
    swapChainCreateInfo.clipped = true;
    swapChainCreateInfo.oldSwapchain = nullptr;

    auto graphicsQueueFamilyIndex = devicePtr->GetQueueFamilyIndex(QueueFamilyType::GRAPHICS);
    auto presentQueueFamilyIndex = devicePtr->GetQueueFamilyIndex(QueueFamilyType::PRESENT);

    uint32_t queueFamilyIndices[2] = { static_cast<uint32_t>(graphicsQueueFamilyIndex),
                                       static_cast<uint32_t>(presentQueueFamilyIndex) };

    if (graphicsQueueFamilyIndex != presentQueueFamilyIndex)
    {
        swapChainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        swapChainCreateInfo.queueFamilyIndexCount = 2;
        swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }

    // Setup swapchain
    auto device = devicePtr->GetVulkanDevice();
    swapChain = device.createSwapchainKHRUnique(swapChainCreateInfo);
    swapChainImages = device.getSwapchainImagesKHR(swapChain.get());
    imageViews.reserve(swapChainImages.size());

    vk::ComponentMapping componentMapping{
        vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA
    };

    vk::ImageSubresourceRange subresourceRange{ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 };
    for (auto image : swapChainImages)
    {
        vk::ImageViewCreateInfo imageViewCreateInfo;
        imageViewCreateInfo.flags = vk::ImageViewCreateFlags();
        imageViewCreateInfo.image = image;
        imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
        imageViewCreateInfo.format = swapChainImageFormat;
        imageViewCreateInfo.components = componentMapping;
        imageViewCreateInfo.subresourceRange = subresourceRange;

        imageViews.emplace_back(device.createImageViewUnique(imageViewCreateInfo));
    }
}

void VulkanSwapChain::CreateBarriers()
{
    vk::SemaphoreCreateInfo semaphoreInfo
    {
        .flags = vk::SemaphoreCreateFlags()
    };

    vk::FenceCreateInfo fenceInfo
    {
        .flags = vk::FenceCreateFlagBits::eSignaled
    };

    auto device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();
    for (size_t i = 0; i < maxFramesInFlight; ++i)
    {
        imageAvailableSemaphores.emplace_back(device.createSemaphoreUnique(semaphoreInfo));
        renderFinishedSemaphores.emplace_back(device.createSemaphoreUnique(semaphoreInfo));
        inFlightFences.emplace_back(device.createFenceUnique(fenceInfo));
    }

    imagesInFlight.resize(swapChainImages.size());

    commandPool = CreateCommandPool(static_cast<uint32_t>(VulkanContext::Get()->GetDevice()->GetQueueFamilyIndex(QueueFamilyType::GRAPHICS)));
    commandBuffers = CreateCommandBuffers(static_cast<uint32_t>(swapChainFramebuffers.size()));
}

void VulkanSwapChain::Present()
{
    return;
}

vk::UniqueCommandPool VulkanSwapChain::CreateCommandPool(uint32_t queueFamilyIndex)
{
    return VulkanContext::Get()->GetDevice()->GetVulkanDevice().createCommandPoolUnique(
        vk::CommandPoolCreateInfo {
            .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
            .queueFamilyIndex = queueFamilyIndex
        }
    );
}

std::vector<vk::UniqueCommandBuffer> VulkanSwapChain::CreateCommandBuffers(uint32_t size)
{
    vk::CommandBufferAllocateInfo commandBufferAllocInfo
    {
        .commandPool = commandPool.get()
        , .level = vk::CommandBufferLevel::ePrimary
        , .commandBufferCount = size
    };

    return VulkanContext::Get()->GetDevice()->GetVulkanDevice().allocateCommandBuffersUnique(commandBufferAllocInfo);
}

void VulkanSwapChain::CreateFramebuffers()
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
            , .renderPass = VulkanContext::Get()->mGraphicsPipeline->GetVulkanRenderPass()
            , .attachmentCount = 1
            , .pAttachments = attachments
            , .width = swapChainExtent.width
            , .height = swapChainExtent.height
            , .layers = 1
        };

        swapChainFramebuffers[i] = VulkanContext::Get()->GetDevice()->GetVulkanDevice().createFramebufferUnique(framebufferCreateInfo);
    }
}

// TODO move to renderer
void VulkanSwapChain::DrawIndexed(vk::Buffer vertexBuffer, vk::Buffer indexBuffer, uint32_t count)
{
    auto mDevice = VulkanContext::Get()->GetDevice();
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

    auto graphicsPipeline = VulkanContext::Get()->mGraphicsPipeline;
    vk::RenderPassBeginInfo renderPassBeginInfo
    {
        .renderPass = graphicsPipeline->GetVulkanRenderPass()
        , .framebuffer = swapChainFramebuffers[imageIndex].get()
        , .renderArea = renderArea
        , .clearValueCount = 1
        , .pClearValues = &clearValues
    };

    commandBuffers[imageIndex]->beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
        commandBuffers[imageIndex]->bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline->GetVulkanPipeline());

        vk::Buffer vertexBuffers[] = { vertexBuffer };
        vk::DeviceSize offsets[] = { 0 };
        commandBuffers[imageIndex]->bindVertexBuffers(0, 1, vertexBuffers, offsets);

        auto& descriptorSets = graphicsPipeline->descriptorSets;
        commandBuffers[imageIndex]->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, graphicsPipeline->GetVulkanPipelineLayout(), 0, 1, &descriptorSets[imageIndex].get(), 0, nullptr);

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

    currentFrame = (currentFrame + 1) % maxFramesInFlight;
}
