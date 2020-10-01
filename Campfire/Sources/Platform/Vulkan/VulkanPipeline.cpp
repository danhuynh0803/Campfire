#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "VulkanPipeline.h"
#include "VulkanShader.h"
#include "VulkanContext.h"

struct PipelineVertex
{
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 uv;

    static vk::VertexInputBindingDescription GetBindingDescription()
    {
        vk::VertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(PipelineVertex);
        bindingDescription.inputRate = vk::VertexInputRate::eVertex;

        return bindingDescription;
    }

    static std::array<vk::VertexInputAttributeDescription, 3> GetAttributeDescriptions()
    {
        std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = vk::Format::eR32G32B32Sfloat;
        attributeDescriptions[0].offset = offsetof(PipelineVertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat;
        attributeDescriptions[1].offset = offsetof(PipelineVertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = vk::Format::eR32G32Sfloat;;
        attributeDescriptions[2].offset = offsetof(PipelineVertex, uv);

        return attributeDescriptions;
    }
};


VulkanPipeline::VulkanPipeline(PipelineType pipelineType)
    : type(pipelineType)
{
    // Create shader modules
    VulkanShader vert("../Campfire/Shaders/vert.spv");
    VulkanShader frag("../Campfire/Shaders/frag.spv");

    // Create pipeline
    vk::PipelineShaderStageCreateInfo vertShaderStageInfo
    {
        .stage = vk::ShaderStageFlagBits::eVertex,
        .module = vert.GetShaderModule(),
        .pName = "main"
    };

    vk::PipelineShaderStageCreateInfo fragShaderStageInfo
    {
        .stage = vk::ShaderStageFlagBits::eFragment,
        .module = frag.GetShaderModule(),
        .pName = "main"
    };

    vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    // Setup fixed function part of pipeline

    // Vertex input
    // Binds = spacing btw data and whether data is per-vertex or per-instance
    // Attribute descriptions = type of the attribs passed to vertex shader,
    // including which binding to load them from and at which offset

    auto bindingDescription = PipelineVertex::GetBindingDescription();
    auto attributeDescriptions = PipelineVertex::GetAttributeDescriptions();

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
    auto swapChainExtent = VulkanContext::Get()->swapChainExtent;
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
        , .cullMode = vk::CullModeFlagBits::eNone
        , .frontFace = vk::FrontFace::eCounterClockwise
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

    // UBO descriptor layout
    vk::DescriptorSetLayoutBinding uboLayoutBinding
    {
        .binding = 0
        , .descriptorType = vk::DescriptorType::eUniformBuffer
        , .descriptorCount = 1
        , .stageFlags = vk::ShaderStageFlagBits::eVertex
        , .pImmutableSamplers = nullptr // used for image sampling later on
    };

    vk::DescriptorSetLayoutCreateInfo layoutInfo
    {
        .bindingCount = 1
        , .pBindings = &uboLayoutBinding
    };

    auto device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();
    descriptorSetLayout = device.createDescriptorSetLayoutUnique(layoutInfo);

    auto swapChainImages = VulkanContext::Get()->swapChainImages;
    vk::DescriptorPoolSize poolSize
    {
        .descriptorCount = static_cast<uint32_t>(swapChainImages.size())
    };

    vk::DescriptorPoolCreateInfo poolInfo
    {
        .maxSets = static_cast<uint32_t>(swapChainImages.size())
        , .poolSizeCount = 1
        , .pPoolSizes = &poolSize
    };

    descriptorPool = device.createDescriptorPoolUnique(poolInfo);

    // Create list of descriptor sets
    std::vector<vk::DescriptorSetLayout> layouts(swapChainImages.size(), descriptorSetLayout.get());
    vk::DescriptorSetAllocateInfo allocInfo
    {
        .descriptorPool = descriptorPool.get()
        , .descriptorSetCount = static_cast<uint32_t>(swapChainImages.size())
        , .pSetLayouts = layouts.data()
    };
    descriptorSets.resize(swapChainImages.size());
    descriptorSets = device.allocateDescriptorSetsUnique(allocInfo);

    // Setup pipeline layout
    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo
    {
        .setLayoutCount = 1
        , .pSetLayouts = &descriptorSetLayout.get()
        , .pushConstantRangeCount = 0
        , .pPushConstantRanges = nullptr
    };
    pipelineLayout = device.createPipelineLayoutUnique(pipelineLayoutCreateInfo);

    // ----------- Setup Renderpasses ---------------------

    // Setup loadOp and StoreOp which determines what to do
    // with the data in the attachment before rendering and
    // after rendering
    vk::AttachmentDescription colorAttachment
    {
        .format = VulkanContext::Get()->swapChainImageFormat
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
    renderPass = device.createRenderPassUnique(renderPassCreateInfo);

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

    pipeline = device.createGraphicsPipelineUnique(nullptr, pipelineCreateInfo);
}
