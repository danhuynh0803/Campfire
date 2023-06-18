#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanPipeline.h"
#include "Vulkan/VulkanInitializers.h"
#include "Vulkan/VulkanUtil.h"
#include "Renderer/FrameGraph.h"

cf::Pipeline::Pipeline(
  const std::vector<std::vector<vk::DescriptorSetLayoutBinding>> & descriptorSetLayoutBindings
, const vk::PipelineShaderStageCreateInfo & shaderStage)
    : Pipeline(
        descriptorSetLayoutBindings,
        std::vector<vk::PipelineShaderStageCreateInfo> { shaderStage },
        PipelineType::eCompute,
        vk::RenderPass{})
{}

cf::Pipeline::Pipeline(
  const std::vector<std::vector<vk::DescriptorSetLayoutBinding>> & descriptorSetLayoutBindings
, const std::vector<vk::PipelineShaderStageCreateInfo> & shaderStages
, PipelineType type
, const vk::RenderPass& renderPass
)
{
    mDevice = VulkanContext::Get()->GetDevice()->GetVulkanDevice();

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
        static_cast<uint32_t>(setLayouts.size()),
        setLayouts.data()
    );

    // Generate descriptor sets associated with each pipeline
    const auto swapChainSize = VulkanContext::Get()->GetSwapChain()->GetImages().size();
    //std::vector<vk::DescriptorSetLayout> layouts { swapChainSize, setLayouts };
    auto allocInfo = vk::initializers::DescriptorSetAllocateInfo(
        VulkanContext::Get()->GetDescriptorPool(),
        static_cast<uint32_t>(setLayouts.size()),
        setLayouts.data());

    mDescriptorSets = mDevice.allocateDescriptorSetsUnique(allocInfo);

    // TODO generalize push descriptors
    if (type == PipelineType::eGraphics)
    {
        vk::PushConstantRange pushConstantRange = {};
        pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eVertex;
        pushConstantRange.size = sizeof(TransformPushConstBlock);
        pushConstantRange.offset = 0;

        pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
        pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
    }
    else if (type == PipelineType::eCompute)
    {
        vk::PushConstantRange pushConstantRange = {};
        pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eCompute;
        pushConstantRange.size = sizeof(glm::ivec4);
        pushConstantRange.offset = 0;

        pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
        pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
    }
    mPipelineLayout = mDevice.createPipelineLayoutUnique(pipelineLayoutCreateInfo);

    switch (type)
    {
    case PipelineType::eGraphics:
    {
        mPipeline = CreateGraphicsPipeline(shaderStages, renderPass);
        break;
    }
    case PipelineType::eCompute:
        mPipeline = CreateComputePipeline(shaderStages);
        break;
    case PipelineType::eRaytracing:
        //mPipeline = CreateRaytracingPipeline();
        break;
    default:
        CORE_ERROR("No matching pipeline type specified during pipeline creation");
        break;
    }
}

namespace cf
{

vk::Format FormatOfVertexComponent(VertexComponent component)
{
    switch (component)
    {
    case VertexComponent::Position:
    case VertexComponent::Normal:
    case VertexComponent::Tangent:
    case VertexComponent::Color:
        return vk::Format::eR32G32B32Sfloat;

    case VertexComponent::UV:
        return vk::Format::eR32G32Sfloat;

    //case VertexComponent::Color:
    //    return vk::Format::eR32G32B32A32Sfloat;

    default:
        return vk::Format::eUndefined;
    }
}

uint32_t SizeOfVertexComponent(VertexComponent component)
{
    switch (component)
    {
    //case VertexComponent::Color:
    //    return sizeof(glm::vec4);
    case VertexComponent::Position:
    case VertexComponent::Normal:
    case VertexComponent::Color:
    case VertexComponent::Tangent:
        return sizeof(glm::vec3);

    case VertexComponent::UV:
        return sizeof(glm::vec2);

    default:
        return 0;
    }
}

vk::PipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo;
vk::VertexInputBindingDescription vertexInputBindingDescription;
std::vector<vk::VertexInputAttributeDescription> vertexInputAttributeDescriptions;

//std::vector<vk::VertexInputAttributeDescription> GetVertexAttributes(uint32_t binding, std::vector<VertexComponent>& components)
//{
//}

vk::PipelineVertexInputStateCreateInfo* CreatePipelineVertexInputState(const std::vector<VertexComponent>& components)
{
    uint32_t location = 0;
    uint32_t offset = 0;
    uint32_t binding = 0;
    vertexInputAttributeDescriptions.clear();

    for (auto component : components)
    {
        vk::VertexInputAttributeDescription desc {};
        desc.location = location;
        desc.binding = binding;
        desc.format = FormatOfVertexComponent(component);
        desc.offset = offset;

        vertexInputAttributeDescriptions.emplace_back(desc);
        location++;
        offset += SizeOfVertexComponent(component);
    }

    vertexInputBindingDescription.binding = binding;
    vertexInputBindingDescription.stride  = offset;
    vertexInputBindingDescription.inputRate = vk::VertexInputRate::eVertex;

    pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
    pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexInputBindingDescription;
    pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = vertexInputAttributeDescriptions.size();
    pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputAttributeDescriptions.data();

    return &pipelineVertexInputStateCreateInfo;
}

struct VertexAttributes
{
    glm::vec3 pos;
    glm::vec2 uv;
    glm::vec3 normal;
};

/*
struct Vertex
{
    glm::vec3 pos;
    glm::vec2 uv;
    glm::vec3 normal;

    inline static vk::PipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo;
    inline static vk::VertexInputBindingDescription vertexInputBindingDescription;
    inline static std::vector<vk::VertexInputAttributeDescription> vertexInputAttributeDescriptions;

    static vk::PipelineVertexInputStateCreateInfo* GetPipelineVertexInputState(const std::vector<VertexComponent>& components)
    {
        vertexInputBindingDescription = vk::initializers::VertexInputBindingDescription(0, sizeof(Vertex), vk::VertexInputRate::eVertex);
        vertexInputAttributeDescriptions.clear();
        uint32_t location = 0;
        for (auto component : components)
        {
            vertexInputAttributeDescriptions.emplace_back(Vertex::InputAttributeDescription(0, location, component));
            location++;
        }

        pipelineVertexInputStateCreateInfo = vk::initializers::PipelineVertexInputStateCreateInfo(
            1, &vertexInputBindingDescription,
            static_cast<uint32_t>(vertexInputAttributeDescriptions.size()),
            vertexInputAttributeDescriptions.data()
        );

        return &pipelineVertexInputStateCreateInfo;
    }

    static vk::VertexInputAttributeDescription InputAttributeDescription(
        uint32_t binding,
        uint32_t location,
        VertexComponent component)
    {
        switch (component)
        {
        case VertexComponent::Position:
            return vk::initializers::VertexInputAttributeDescription(binding, location, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, pos));
        case VertexComponent::UV:
            return vk::initializers::VertexInputAttributeDescription(binding, location, vk::Format::eR32G32Sfloat, offsetof(Vertex, uv));
        case VertexComponent::Normal:
            return vk::initializers::VertexInputAttributeDescription(binding, location, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, normal));
        default:
            CORE_ERROR("No valid Input Attribute for specified component");
        }
    }
};
*/

} // Namespace cf

// ====================================================================================
// Graphics pipeline
vk::UniquePipeline cf::Pipeline::CreateGraphicsPipeline(
    const std::vector<vk::PipelineShaderStageCreateInfo> & shaderStages
  , const vk::RenderPass& renderPass
)
{
    // Input assembly
    auto inputAssemblyStateCreateInfo =
        vk::initializers::PipelineInputAssemblyStateCreateInfo(vk::PrimitiveTopology::eTriangleList, VK_FALSE);

    // Setup viewports and scissor rect
    auto swapChain = VulkanContext::Get()->GetSwapChain();
    auto swapChainExtent = swapChain->GetExtent();
    auto viewport =
        vk::initializers::Viewport(0.0f, 0.0f, swapChainExtent.width, swapChainExtent.height, 0.0f, 1.0);

    vk::Rect2D scissors;
    scissors.offset = VkOffset2D{0, 0};
    scissors.extent = swapChainExtent;

    auto viewportStateCreateInfo =
        vk::initializers::PipelineViewportStateCreateInfo(1, &viewport, 1, &scissors);

    // Setup rasterizer
    auto rasterizationStateCreateInfo =
        vk::initializers::PipelineRasterizationStateCreateInfo(
            vk::PolygonMode::eFill,
            vk::CullModeFlagBits::eNone,
            vk::FrontFace::eCounterClockwise);

    // Multisampling
    auto multisampleCreateInfo =
        vk::initializers::PipelineMultisampleStateCreateInfo(vk::SampleCountFlagBits::e1);

    // Depth and stencil operators
    auto depthStencilCreateInfo =
        vk::initializers::PipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, vk::CompareOp::eLess);

    // Setup color blending
    auto colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    auto colorBlendAttachment = vk::initializers::PipelineColorBlendAttachmentState(colorWriteMask, VK_FALSE);
    auto colorBlendState = vk::initializers::PipelineColorBlendStateCreateInfo(1, &colorBlendAttachment);

    // Setup dynamic state - these can be changed without recreating the pipeline
    std::vector<vk::DynamicState> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor, vk::DynamicState::eLineWidth };

    auto dynamicStateCreateInfo = vk::initializers::PipelineDynamicStateCreateInfo(static_cast<uint32_t>(dynamicStates.size()), dynamicStates.data());

    // Create Graphics pipeline
    vk::GraphicsPipelineCreateInfo pipelineCreateInfo;
    pipelineCreateInfo.flags = vk::PipelineCreateFlags();
    pipelineCreateInfo.stageCount = shaderStages.size();
    pipelineCreateInfo.pStages = shaderStages.data();
    {
        using namespace cf;
        //pipelineCreateInfo.pVertexInputState = Vertex::GetPipelineVertexInputState({ VertexComponent::Position, VertexComponent::UV, VertexComponent::Normal });
        pipelineCreateInfo.pVertexInputState = CreatePipelineVertexInputState({ VertexComponent::Position, VertexComponent::UV, VertexComponent::Normal });
    }
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
    pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
    pipelineCreateInfo.pMultisampleState = &multisampleCreateInfo;
    pipelineCreateInfo.pDepthStencilState = &depthStencilCreateInfo;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
    pipelineCreateInfo.layout = mPipelineLayout.get();
    pipelineCreateInfo.renderPass = renderPass;
    pipelineCreateInfo.subpass = 0;
    pipelineCreateInfo.basePipelineHandle = nullptr;
    pipelineCreateInfo.basePipelineIndex = -1;

    return mDevice.createGraphicsPipelineUnique(nullptr, pipelineCreateInfo);
}

// ====================================================================================
// Compute pipeline
vk::UniquePipeline cf::Pipeline::CreateComputePipeline(
    const std::vector<vk::PipelineShaderStageCreateInfo> & shaderStages)
{
    vk::ComputePipelineCreateInfo pipelineCreateInfo {};
    // Assume compute shader is the first shader in the list since pipeline can only use one shader
    pipelineCreateInfo.stage = shaderStages.at(0);
    pipelineCreateInfo.layout = mPipelineLayout.get();
    // TODO cache for quicker perf
    vk::PipelineCache pipelineCache;
    return mDevice.createComputePipelineUnique(pipelineCache, pipelineCreateInfo);
}
