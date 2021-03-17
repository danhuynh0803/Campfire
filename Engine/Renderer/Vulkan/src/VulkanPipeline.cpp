#include "Vulkan/VulkanPipeline.h"
#include "Vulkan/VulkanShader.h"
#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanUtil.h"
#include "Vulkan/VulkanInitializers.h"

#include "Core/ResourceManager.h" // For asset directory macros

struct PipelineVertex
{
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 uv;
};

VulkanPipeline::VulkanPipeline(PipelineType pipelineType)
    : type(pipelineType)
{
    // Setup fixed function part of pipeline

    // Vertex input
    // Binds = spacing btw data and whether data is per-vertex or per-instance
    // Attribute descriptions = type of the attribs passed to vertex shader,
    // including which binding to load them from and at which offset
    auto bindingDescription = vk::initializers::VertexInputBindingDescription(0, sizeof(PipelineVertex), vk::VertexInputRate::eVertex);

    std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions{};
    attributeDescriptions[0] = vk::initializers::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, offsetof(PipelineVertex, pos));
    attributeDescriptions[1] = vk::initializers::VertexInputAttributeDescription(0, 1, vk::Format::eR32G32B32Sfloat, offsetof(PipelineVertex, color));
    attributeDescriptions[2] = vk::initializers::VertexInputAttributeDescription(0, 2, vk::Format::eR32G32Sfloat, offsetof(PipelineVertex, uv));

    vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
    vertexInputStateCreateInfo.flags = vk::PipelineVertexInputStateCreateFlags();
    vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
    vertexInputStateCreateInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputStateCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    // Input assembly
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;
    inputAssemblyStateCreateInfo.flags = vk::PipelineInputAssemblyStateCreateFlags();
    inputAssemblyStateCreateInfo.topology = vk::PrimitiveTopology::eTriangleList;
    inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

    // Setup viewports and scissor rect
    auto swapChain = VulkanContext::Get()->GetSwapChain();
    auto swapChainExtent = swapChain->GetExtent();
    vk::Viewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapChainExtent.width);
    viewport.height = static_cast<float>(swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vk::Rect2D scissors;
    scissors.offset = {0, 0};
    scissors.extent = swapChainExtent;

    vk::PipelineViewportStateCreateInfo viewportStateCreateInfo;
    viewportStateCreateInfo.flags = vk::PipelineViewportStateCreateFlags();
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.pViewports = &viewport;
    viewportStateCreateInfo.scissorCount = 1;
    viewportStateCreateInfo.pScissors = &scissors;

    // Setup rasterizer
    // Note: depthClampEnable: if true, fragments beyond
    // near and far planes will be clamped instead of being discard
    vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
    rasterizationStateCreateInfo.flags = vk::PipelineRasterizationStateCreateFlags();
    rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
    rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationStateCreateInfo.polygonMode = vk::PolygonMode::eFill;
    rasterizationStateCreateInfo.cullMode = vk::CullModeFlagBits::eNone;
    rasterizationStateCreateInfo.frontFace = vk::FrontFace::eCounterClockwise;
    rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
    rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
    rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
    rasterizationStateCreateInfo.lineWidth = 1.0f;

    // Enable multisampling
    vk::PipelineMultisampleStateCreateInfo multisampleCreateInfo;
    multisampleCreateInfo.flags = vk::PipelineMultisampleStateCreateFlags();
    multisampleCreateInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;
    multisampleCreateInfo.sampleShadingEnable = VK_FALSE;
    multisampleCreateInfo.minSampleShading = 1.0f;
    multisampleCreateInfo.pSampleMask = nullptr;
    multisampleCreateInfo.alphaToCoverageEnable = VK_FALSE;
    multisampleCreateInfo.alphaToOneEnable = VK_FALSE;

    // Depth and stencil operators
    vk::PipelineDepthStencilStateCreateInfo depthStencilCreateInfo {};
    depthStencilCreateInfo.depthTestEnable = true;
    depthStencilCreateInfo.depthWriteEnable = true;
    depthStencilCreateInfo.depthCompareOp = vk::CompareOp::eLess;
    depthStencilCreateInfo.depthBoundsTestEnable = false;
    depthStencilCreateInfo.minDepthBounds = 0.0f;
    depthStencilCreateInfo.maxDepthBounds = 1.0f;
    depthStencilCreateInfo.stencilTestEnable = false;
    depthStencilCreateInfo.front = {};
    depthStencilCreateInfo.back = {};

    // Setup color blending
    vk::PipelineColorBlendAttachmentState colorBlendAttachment;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne;
    colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero;
    colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
    colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
    colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
    colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;
    colorBlendAttachment.colorWriteMask =
        vk::ColorComponentFlagBits::eR
        | vk::ColorComponentFlagBits::eG
        | vk::ColorComponentFlagBits::eB
        | vk::ColorComponentFlagBits::eA
    ;

    vk::PipelineColorBlendStateCreateInfo colorBlendCreateInfo;
    colorBlendCreateInfo.flags = vk::PipelineColorBlendStateCreateFlags();
    colorBlendCreateInfo.logicOpEnable = VK_FALSE;
    colorBlendCreateInfo.logicOp = vk::LogicOp::eCopy;
    colorBlendCreateInfo.attachmentCount = 1;
    colorBlendCreateInfo.pAttachments = &colorBlendAttachment;

    // Setup dynamic state - these can be changed without recreating the pipeline
    std::vector<vk::DynamicState> dynamicStates =
    {
        vk::DynamicState::eViewport,
        vk::DynamicState::eLineWidth,
    };

    vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo;
    dynamicStateCreateInfo.flags = vk::PipelineDynamicStateCreateFlags();
    dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

    std::vector<vk::DescriptorSetLayoutBinding> layoutBindings;
    { // UBO descriptor layout
        for (size_t i = 0; i < 2; ++i)
        {
            vk::DescriptorSetLayoutBinding uboLayoutBinding;
            uboLayoutBinding.binding = static_cast<uint32_t>(i);
            uboLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
            uboLayoutBinding.descriptorCount = 1;
            uboLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;
            uboLayoutBinding.pImmutableSamplers = nullptr; // used for image sampling later on

            layoutBindings.emplace_back(uboLayoutBinding);
        }
    }

    { // Sampler layout binding
        vk::DescriptorSetLayoutBinding samplerLayoutBinding {};
        samplerLayoutBinding.binding = 2;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;
        layoutBindings.emplace_back(samplerLayoutBinding);
    }


    vk::DescriptorSetLayoutCreateInfo layoutInfo;
    layoutInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
    layoutInfo.pBindings = layoutBindings.data();

    auto device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();
    descriptorSetLayout = device.createDescriptorSetLayoutUnique(layoutInfo);
    auto swapChainImages = VulkanContext::Get()->GetSwapChain()->GetImages();

    { // Creating descriptor pools
        std::array<vk::DescriptorPoolSize, 2> poolSizes {};
        poolSizes[0].type = vk::DescriptorType::eUniformBuffer;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(swapChainImages.size());
        poolSizes[1].type = vk::DescriptorType::eCombinedImageSampler;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(swapChainImages.size());

        vk::DescriptorPoolCreateInfo poolInfo;
        poolInfo.maxSets = static_cast<uint32_t>(swapChainImages.size());
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();

        descriptorPool = device.createDescriptorPoolUnique(poolInfo);
    }

    // Create list of descriptor sets
    std::vector<vk::DescriptorSetLayout> layouts(swapChainImages.size(), descriptorSetLayout.get());
    vk::DescriptorSetAllocateInfo allocInfo;
    allocInfo.descriptorPool = descriptorPool.get();
    allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(swapChainImages.size());
    descriptorSets = device.allocateDescriptorSetsUnique(allocInfo);

    // Setup pipeline layout
    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout.get();
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

    pipelineLayout = device.createPipelineLayoutUnique(pipelineLayoutCreateInfo);

    // ----------- Setup Renderpasses ---------------------

    // Setup loadOp and StoreOp which determines what to do
    // with the data in the attachment before rendering and
    // after rendering

    // ==============================
    // Color Attachment
    vk::AttachmentDescription colorAttachment;
    colorAttachment.format = VulkanContext::Get()->GetSwapChain()->GetFormat();
    colorAttachment.samples = vk::SampleCountFlagBits::e1;
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear; // Clear the values to a constant at start
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore; // Rendered contents store in memory and can be read later
    colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;
    // ==============================

    // ==============================
    // Depth Attachment
    vk::AttachmentDescription depthAttachment;
    depthAttachment.format = vkUtil::FindDepthFormat();
    depthAttachment.samples = vk::SampleCountFlagBits::e1;
    depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
    depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
    depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    // ==============================

    // ==============================
    // Setup subpasses
    vk::AttachmentReference colorAttachmentRef;
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::AttachmentReference depthAttachmentRef;
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    vk::SubpassDescription subpass;
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    // Setup subpass dependencies
    vk::SubpassDependency subpassDependency;
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
    subpassDependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
    subpassDependency.srcAccessMask = static_cast<vk::AccessFlagBits>(0);
    subpassDependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
    // ==============================

    // ==============================
    // Setup render pass
    std::array<vk::AttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
    vk::RenderPassCreateInfo renderPassCreateInfo {};
    renderPassCreateInfo.flags = vk::RenderPassCreateFlags();
    renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassCreateInfo.pAttachments = attachments.data();
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpass;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &subpassDependency;

    renderPass = device.createRenderPassUnique(renderPassCreateInfo);
    // ==============================

    // Shaders
    auto vertShaderStageInfo = vk::initializers::PipelineShaderStageCreateInfo(SHADERS + "/vert.spv", vk::ShaderStageFlagBits::eVertex);
    auto fragShaderStageInfo = vk::initializers::PipelineShaderStageCreateInfo(SHADERS + "/frag.spv", vk::ShaderStageFlagBits::eFragment);
    vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    // ==============================
    // Create Graphics pipeline
    vk::GraphicsPipelineCreateInfo pipelineCreateInfo;
    pipelineCreateInfo.flags = vk::PipelineCreateFlags();
    pipelineCreateInfo.stageCount = 2;
    pipelineCreateInfo.pStages = shaderStages;
    pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
    pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
    pipelineCreateInfo.pMultisampleState = &multisampleCreateInfo;
    pipelineCreateInfo.pDepthStencilState = &depthStencilCreateInfo;
    pipelineCreateInfo.pColorBlendState = &colorBlendCreateInfo;
    pipelineCreateInfo.pDynamicState = nullptr;
    pipelineCreateInfo.layout = pipelineLayout.get();
    pipelineCreateInfo.renderPass = renderPass.get();
    pipelineCreateInfo.subpass = 0;
    pipelineCreateInfo.basePipelineHandle = nullptr;
    pipelineCreateInfo.basePipelineIndex = -1;

    pipeline = device.createGraphicsPipelineUnique(nullptr, pipelineCreateInfo);
    // ==============================
}

// FIXME, pretty much the pipeline constructor code
// but removes the descriptor layout setting which isnt needed when remaking on resize
// Also causes a crash for some reason when trying to set pipeline layout
void VulkanPipeline::RecreatePipeline()
{
    // TODO
    return;
}


