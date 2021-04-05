#include "Vulkan/VulkanPipeline.h"
#include "Vulkan/VulkanShader.h"
#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanUtil.h"
#include "Vulkan/VulkanInitializers.h"

#include "Core/ResourceManager.h" // For asset directory macros

namespace
{
    std::vector<vk::DescriptorSetLayout> GetDescriptorSetLayouts(
        const std::vector<vk::UniqueDescriptorSetLayout>& uniqueLayouts
    )
    {
        std::vector<vk::DescriptorSetLayout> layouts;
        for (auto& uniqueLayout : uniqueLayouts)
        {
            layouts.emplace_back(uniqueLayout.get());
        }

        return layouts;
    }
}

struct PipelineVertex
{
    glm::vec3 pos;
    glm::vec2 uv;
    glm::vec3 normal;
};

SharedPtr<VulkanPipeline> VulkanPipeline::Create(PipelineType type)
{
    switch (type)
    {
        case PipelineType::GRAPHICS:
            return CreateSharedPtr<GraphicsPipeline>();
        case PipelineType::COMPUTE:
            return CreateSharedPtr<ComputePipeline>();
        default:
            CORE_ERROR("Invalid PipelineType");
    }

    CORE_ERROR("Could not create pipeline");
    return nullptr;
}

GraphicsPipeline::GraphicsPipeline()
{
    auto device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();

    // Setup fixed function part of pipeline

    // Vertex input
    // Binds = spacing btw data and whether data is per-vertex or per-instance
    // Attribute descriptions = type of the attribs passed to vertex shader,
    // including which binding to load them from and at which offset
    auto bindingDescription = vk::initializers::VertexInputBindingDescription(0, sizeof(PipelineVertex), vk::VertexInputRate::eVertex);

    std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions{};
    attributeDescriptions[0] = vk::initializers::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, offsetof(PipelineVertex, pos));
    attributeDescriptions[1] = vk::initializers::VertexInputAttributeDescription(0, 1, vk::Format::eR32G32Sfloat, offsetof(PipelineVertex, uv));
    attributeDescriptions[2] = vk::initializers::VertexInputAttributeDescription(0, 2, vk::Format::eR32G32B32Sfloat, offsetof(PipelineVertex, normal));

    auto vertexInputStateCreateInfo = vk::initializers::PipelineVertexInputStateCreateInfo(
        1, &bindingDescription,
        static_cast<uint32_t>(attributeDescriptions.size()), attributeDescriptions.data()
    );

    // Input assembly
    auto inputAssemblyStateCreateInfo = vk::initializers::PipelineInputAssemblyStateCreateInfo(vk::PrimitiveTopology::eTriangleList, VK_FALSE);

    // Setup viewports and scissor rect
    auto swapChain = VulkanContext::Get()->GetSwapChain();
    auto swapChainExtent = swapChain->GetExtent();
    auto viewport = vk::initializers::Viewport(0.0f, 0.0f, swapChainExtent.width, swapChainExtent.height, 0.0f, 1.0);

    vk::Rect2D scissors;
    scissors.offset = {0, 0};
    scissors.extent = swapChainExtent;

    auto viewportStateCreateInfo = vk::initializers::PipelineViewportStateCreateInfo(1, &viewport, 1, &scissors);

    // Setup rasterizer
    auto rasterizationStateCreateInfo = vk::initializers::PipelineRasterizationStateCreateInfo(vk::PolygonMode::eFill, vk::CullModeFlagBits::eNone, vk::FrontFace::eCounterClockwise);

    // Multisampling
    auto multisampleCreateInfo = vk::initializers::PipelineMultisampleStateCreateInfo(vk::SampleCountFlagBits::e1);

    // Depth and stencil operators
    auto depthStencilCreateInfo = vk::initializers::PipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, vk::CompareOp::eLess);

    // Setup color blending
    auto colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    auto colorBlendAttachment = vk::initializers::PipelineColorBlendAttachmentState(colorWriteMask, VK_FALSE);
    auto colorBlendState = vk::initializers::PipelineColorBlendStateCreateInfo(1, &colorBlendAttachment);

    // Setup dynamic state - these can be changed without recreating the pipeline
    std::vector<vk::DynamicState> dynamicStates =
    {
        vk::DynamicState::eViewport,
        vk::DynamicState::eLineWidth,
    };

    auto dynamicState = vk::initializers::PipelineDynamicStateCreateInfo(
        static_cast<uint32_t>(dynamicStates.size()),
        dynamicStates.data()
    );

    // Setup descriptorlayout and descriptor sets (doesnt have to be part of pipeline creation)
    SetupDescriptors();

    std::vector<vk::DescriptorSetLayout> descriptorLayouts = GetDescriptorSetLayouts(descriptorSetLayouts);

    // Setup pipeline layout
    auto pipelineLayoutCreateInfo = vk::initializers::PipelineLayoutCreateInfo(
        static_cast<uint32_t>(descriptorLayouts.size()),
        descriptorLayouts.data());

    vk::PushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eVertex;
    pushConstantRange.size = sizeof(TransformPushConstBlock);
    pushConstantRange.offset = 0;

    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

    pipelineLayout = device.createPipelineLayoutUnique(pipelineLayoutCreateInfo);

    SetupRenderPass();

    // Shaders
    auto vertShaderStageInfo = vk::initializers::PipelineShaderStageCreateInfo(SHADERS + "/vert.spv", vk::ShaderStageFlagBits::eVertex);
    auto fragShaderStageInfo = vk::initializers::PipelineShaderStageCreateInfo(SHADERS + "/frag.spv", vk::ShaderStageFlagBits::eFragment);
    vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

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
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pDynamicState = nullptr;
    pipelineCreateInfo.layout = pipelineLayout.get();
    pipelineCreateInfo.renderPass = renderPass.get();
    pipelineCreateInfo.subpass = 0;
    pipelineCreateInfo.basePipelineHandle = nullptr;
    pipelineCreateInfo.basePipelineIndex = -1;

    pipeline = device.createGraphicsPipelineUnique(nullptr, pipelineCreateInfo);
}

void GraphicsPipeline::SetupDescriptors()
{
    auto device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();
    auto swapChainImages = VulkanContext::Get()->GetSwapChain()->GetImages();
    { // Creating descriptor pools
        std::array<vk::DescriptorPoolSize, 2> poolSizes {};
        poolSizes[0].type = vk::DescriptorType::eUniformBuffer;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(swapChainImages.size());
        poolSizes[1].type = vk::DescriptorType::eCombinedImageSampler;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(swapChainImages.size());

        vk::DescriptorPoolCreateInfo poolInfo;
        // TODO have poolsize be configurable
        poolInfo.maxSets = 10000 * static_cast<uint32_t>(swapChainImages.size());
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();

        descriptorPool = device.createDescriptorPoolUnique(poolInfo);
    }

    { // UBOs
        // Camera
        auto camera = vk::initializers::DescriptorSetLayoutBinding(
            vk::DescriptorType::eUniformBuffer,
            vk::ShaderStageFlagBits::eVertex,
            0);

        // Lights
        auto lights = vk::initializers::DescriptorSetLayoutBinding(
            vk::DescriptorType::eUniformBuffer,
            vk::ShaderStageFlagBits::eFragment,
            1);

        std::vector<vk::DescriptorSetLayoutBinding> layoutBindings {
            camera,
            lights,
        };

        auto descriptorSetLayoutInfo = vk::initializers::DescriptorSetLayoutCreateInfo(
            static_cast<uint32_t>(layoutBindings.size()),
            layoutBindings.data());

        // Set 0
        descriptorSetLayouts.emplace_back(device.createDescriptorSetLayoutUnique(descriptorSetLayoutInfo));
   }


    { // Material descriptors
        // Albedo map
        auto albedo = vk::initializers::DescriptorSetLayoutBinding(
            vk::DescriptorType::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment,
            0);

        // Normal map
        auto normal = vk::initializers::DescriptorSetLayoutBinding(
            vk::DescriptorType::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment,
            1);

        std::vector<vk::DescriptorSetLayoutBinding> layoutBindings {
            albedo,
            normal,
        };

        auto descriptorSetLayoutInfo = vk::initializers::DescriptorSetLayoutCreateInfo(
            static_cast<uint32_t>(layoutBindings.size()),
            layoutBindings.data());

        // Set 1
        descriptorSetLayouts.emplace_back(device.createDescriptorSetLayoutUnique(descriptorSetLayoutInfo));
    }

    // DescriptorSets
    std::vector<vk::DescriptorSetLayout> sets = GetDescriptorSetLayouts(descriptorSetLayouts);    
    auto allocInfo = vk::initializers::DescriptorSetAllocateInfo(
        descriptorPool.get(),
        static_cast<uint32_t>(descriptorSetLayouts.size()),
        sets.data()
    );

    descriptorSets = device.allocateDescriptorSetsUnique(allocInfo);
}

void GraphicsPipeline::SetupRenderPass()
{
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

    vk::AttachmentReference colorAttachmentRef;
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

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

    // Setup subpasses
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

    auto device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();
    renderPass = device.createRenderPassUnique(renderPassCreateInfo);
}


// FIXME, pretty much the pipeline constructor code
// but removes the descriptor layout setting which isnt needed when remaking on resize
// Also causes a crash for some reason when trying to set pipeline layout
void GraphicsPipeline::RecreatePipeline()
{
    return;
}


ComputePipeline::ComputePipeline()
{
}

void ComputePipeline::RecreatePipeline()
{
    return;
}
