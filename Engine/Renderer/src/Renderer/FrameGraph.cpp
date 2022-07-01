#include "Renderer/FrameGraph.h"
#include "Core/ResourceManager.h"
#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanInitializers.h"
#include "Vulkan/VulkanShader.h"
#include "Vulkan/VulkanUtil.h"

#include <vector>

FrameGraph::FrameGraph()
{
    // TODO move to a renderer specifically
}

void FrameGraph::CreateRenderFrameGraph()
{
    mDevice = VulkanContext::Get()->GetDevice()->GetVulkanDevice();

    // Prepare renderpasses
    CreateOpaque();

    // Prepare pipelines
    PrepareGraphicsPipeline();
}

void FrameGraph::ReconstructFrameGraph()
{
    PrepareGraphicsPipeline();
}

SharedPtr<VulkanGraphicsPipeline> CreateModelPipeline()
{
    auto swapChainImages = VulkanContext::Get()->GetSwapChain()->GetImages();

    std::vector<std::vector<vk::DescriptorSetLayoutBinding>> descriptorSetLayoutBindings(2);
    { // Environment descriptors
        // Camera
        auto camera = vk::initializers::DescriptorSetLayoutBinding(
            vk::DescriptorType::eUniformBuffer,
            vk::ShaderStageFlagBits::eAllGraphics,
            0);

        // TODO switch to storage buffer
        // Lights
        auto lights = vk::initializers::DescriptorSetLayoutBinding(
            vk::DescriptorType::eUniformBuffer,
            vk::ShaderStageFlagBits::eFragment,
            1);

        // Set 0
        descriptorSetLayoutBindings[0] = {
            camera,
            lights,
        };
    }

    { // Material descriptors
        // Albedo map (or computeResolve)
        auto albedo = vk::initializers::DescriptorSetLayoutBinding(
            vk::DescriptorType::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment,
            0);

        // Metallic map
        auto metallic = vk::initializers::DescriptorSetLayoutBinding(
            vk::DescriptorType::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment,
            1);

        // Normal map
        auto normal = vk::initializers::DescriptorSetLayoutBinding(
            vk::DescriptorType::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment,
            2);

        // Roughness map
        auto roughness = vk::initializers::DescriptorSetLayoutBinding(
            vk::DescriptorType::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment,
            3);

        // Ambient Occlusion map
        auto ambientOcclusion = vk::initializers::DescriptorSetLayoutBinding(
            vk::DescriptorType::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment,
            4);

        // Emissive map
        auto emissive = vk::initializers::DescriptorSetLayoutBinding(
            vk::DescriptorType::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment,
            5);

        // TextureMap usage
        auto mapUsage = vk::initializers::DescriptorSetLayoutBinding(
            vk::DescriptorType::eUniformBuffer,
            vk::ShaderStageFlagBits::eFragment,
            6);

        descriptorSetLayoutBindings[1] = {
            albedo,
            metallic,
            normal,
            roughness,
            ambientOcclusion,
            emissive,
            mapUsage,
        };
    }

    // Shaders
    auto vs = CreateSharedPtr<VulkanShader>(SHADERS + "/model.vert");
    vk::PipelineShaderStageCreateInfo vsStageInfo{};
    vsStageInfo.stage  = vk::ShaderStageFlagBits::eVertex;
    vsStageInfo.module = vs->GetShaderModule();
    vsStageInfo.pName  = "main";

    auto fs = CreateSharedPtr<VulkanShader>(SHADERS + "/model.frag");
    vk::PipelineShaderStageCreateInfo fsStageInfo{};
    fsStageInfo.stage  = vk::ShaderStageFlagBits::eFragment;
    fsStageInfo.module = fs->GetShaderModule();
    fsStageInfo.pName  = "main";

    //auto vertShaderStageInfo = vk::initializers::PipelineShaderStageCreateInfo(
    //    SHADERS + "/model.vert.spv"
    //  , vk::ShaderStageFlagBits::eVertex
    //);

    //auto fragShaderStageInfo = vk::initializers::PipelineShaderStageCreateInfo(
    //    SHADERS + "/model.frag.spv"
    //  , vk::ShaderStageFlagBits::eFragment
    //);

    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages = {
        vsStageInfo,
        fsStageInfo,
    };

    return CreateSharedPtr<VulkanGraphicsPipeline>(
        descriptorSetLayoutBindings,
        shaderStages
    );
}

void FrameGraph::PrepareGraphicsPipeline()
{
    mGraphicsPipelines["models"] = CreateModelPipeline();
}

RenderPass& AddRenderPass(
    const std::string& label,
    RenderQueue queue)
{
    RenderPass pass;

    return pass;
}

void FrameGraph::CreateOpaque()
{
    const std::string label = "opaque";

    // Color Attachment
    vk::AttachmentDescription colorAttachment;
    colorAttachment.format = VulkanContext::Get()->GetSwapChain()->GetFormat();
    colorAttachment.samples = vk::SampleCountFlagBits::e1;
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear; // Clear the values to a constant at start
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore; // Rendered contents store in memory and can be read later
    colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachment.finalLayout = vk::ImageLayout::eColorAttachmentOptimal;

    // Depth Attachment
    vk::AttachmentDescription depthAttachment;
    depthAttachment.format = vk::util::FindDepthFormat();
    depthAttachment.samples = vk::SampleCountFlagBits::e1;
    depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
    depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
    depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    vk::AttachmentReference depthAttachmentRef;
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    vk::AttachmentReference colorAttachmentRef;
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

    // Setup subpasses
    std::vector<vk::SubpassDescription> subpasses;
    {
        vk::SubpassDescription subpass;
        subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;
        subpasses.emplace_back(subpass);
    }

    // Setup subpass dependencies
    std::vector<vk::SubpassDependency> subpassDependencies;
    {
        vk::SubpassDependency dep;
        dep.srcSubpass = VK_SUBPASS_EXTERNAL;
        dep.dstSubpass = 0;
        dep.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
        dep.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
        dep.srcAccessMask = static_cast<vk::AccessFlagBits>(0);
        dep.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
        subpassDependencies.emplace_back(dep);
    }

    // Setup render pass
    std::array<vk::AttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
    vk::RenderPassCreateInfo renderPassCreateInfo {};
    renderPassCreateInfo.flags = vk::RenderPassCreateFlags();
    renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassCreateInfo.pAttachments = attachments.data();
    renderPassCreateInfo.subpassCount = static_cast<uint32_t>(subpasses.size());
    renderPassCreateInfo.pSubpasses = subpasses.data();
    renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());
    renderPassCreateInfo.pDependencies = subpassDependencies.data();

    mRenderPasses.emplace(label, mDevice.createRenderPassUnique(renderPassCreateInfo));
}
