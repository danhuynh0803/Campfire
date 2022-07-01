#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanPipeline.h"
#include "Vulkan/VulkanInitializers.h"
#include "Vulkan/VulkanUtil.h"

cf::Pipeline::Pipeline(
  const std::vector<std::vector<vk::DescriptorSetLayoutBinding>> & descriptorSetLayoutBindings
, const std::vector<vk::PipelineShaderStageCreateInfo> & shaderStages
, PipelineType type)
{
    // Create UniqueDescriptorSetLayouts from the layout bindings parameter
    for (auto setBindings : descriptorSetLayoutBindings)
    {
        auto descriptorSetLayoutInfo = vk::initializers::DescriptorSetLayoutCreateInfo(
            static_cast<uint32_t>(setBindings.size()),
            setBindings.data()
        );

        mDescriptorSetLayouts.emplace_back(
            mDevice.createDescriptorSetLayoutUnique(descriptorSetLayoutInfo)
        );
    }

    std::vector<vk::DescriptorSetLayout> setLayouts = vk::util::ConvertUnique(mDescriptorSetLayouts);
    // Setup pipeline layout
    auto pipelineLayoutCreateInfo = vk::initializers::PipelineLayoutCreateInfo(
        static_cast<uint32_t>(setLayouts.size())
      , setLayouts.data()
    );

    vk::PushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eVertex;
    pushConstantRange.size = sizeof(TransformPushConstBlock);
    pushConstantRange.offset = 0;

    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

    mPipelineLayout = mDevice.createPipelineLayoutUnique(pipelineLayoutCreateInfo);

    switch (type)
    {
    default:
    case PipelineType::eGraphics:
        //mPipeline = CreateGraphicsPipeline();
        break;

    case PipelineType::eCompute:
        //mPipeline = CreateComputePipeline();
        break;
    }
}

vk::UniquePipeline cf::Pipeline::CreateGraphicsPipeline(
  const std::vector<std::vector<vk::DescriptorSetLayoutBinding>> & descriptorSetLayoutBindings
, const std::vector<vk::PipelineShaderStageCreateInfo> & shaderStages)
{
    auto device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();

    // Input assembly
    auto inputAssemblyStateCreateInfo = vk::initializers::PipelineInputAssemblyStateCreateInfo(vk::PrimitiveTopology::eTriangleList, VK_FALSE);

    // Setup viewports and scissor rect
    auto swapChain = VulkanContext::Get()->GetSwapChain();
    auto swapChainExtent = swapChain->GetExtent();
    auto viewport = vk::initializers::Viewport(0.0f, 0.0f, swapChainExtent.width, swapChainExtent.height, 0.0f, 1.0);

    vk::Rect2D scissors;
    scissors.offset = VkOffset2D{0, 0};
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
    std::vector<vk::DynamicState> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor, vk::DynamicState::eLineWidth };

    auto dynamicStateCreateInfo = vk::initializers::PipelineDynamicStateCreateInfo(static_cast<uint32_t>(dynamicStates.size()), dynamicStates.data());

    // Create UniqueDescriptorSetLayouts from the layout bindings parameter
    for (auto setBindings : descriptorSetLayoutBindings)
    {
        auto descriptorSetLayoutInfo = vk::initializers::DescriptorSetLayoutCreateInfo(
            static_cast<uint32_t>(setBindings.size()),
            setBindings.data()
        );

        mDescriptorSetLayouts.emplace_back(
            device.createDescriptorSetLayoutUnique(descriptorSetLayoutInfo)
        );
    }

    std::vector<vk::DescriptorSetLayout> setLayouts = vk::util::ConvertUnique(mDescriptorSetLayouts);
    // Setup pipeline layout
    auto pipelineLayoutCreateInfo = vk::initializers::PipelineLayoutCreateInfo(
        static_cast<uint32_t>(setLayouts.size())
      , setLayouts.data()
    );

    vk::PushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eVertex;
    pushConstantRange.size = sizeof(TransformPushConstBlock);
    pushConstantRange.offset = 0;

    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

    mPipelineLayout = device.createPipelineLayoutUnique(pipelineLayoutCreateInfo);

    // Create Graphics pipeline
    vk::GraphicsPipelineCreateInfo pipelineCreateInfo;
    pipelineCreateInfo.flags = vk::PipelineCreateFlags();
    pipelineCreateInfo.stageCount = shaderStages.size();
    pipelineCreateInfo.pStages = shaderStages.data();
    pipelineCreateInfo.pVertexInputState = Vertex::GetPipelineVertexInputState({ VertexComponent::Position, VertexComponent::UV, VertexComponent::Normal });
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
    pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
    pipelineCreateInfo.pMultisampleState = &multisampleCreateInfo;
    pipelineCreateInfo.pDepthStencilState = &depthStencilCreateInfo;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
    pipelineCreateInfo.layout = mPipelineLayout.get();
    pipelineCreateInfo.renderPass = VulkanContext::Get()->mFrameGraph->GetRenderPass("opaque");
    pipelineCreateInfo.subpass = 0;
    pipelineCreateInfo.basePipelineHandle = nullptr;
    pipelineCreateInfo.basePipelineIndex = -1;

    return device.createGraphicsPipelineUnique(nullptr, pipelineCreateInfo);
}
