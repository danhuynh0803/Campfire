#include "ImGui/VulkanImGui.h"
#include "Vulkan/VulkanUtil.h"
#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanShader.h"
#include "Core/ResourceManager.h"
#include "Vulkan/VulkanInitializers.h"

VulkanImGui::VulkanImGui()
{
    InitResources();
}

void VulkanImGui::UpdateBuffers()
{
    ImDrawData* imDrawData = ImGui::GetDrawData();

    vk::DeviceSize vertexBufferSize = imDrawData->TotalVtxCount * sizeof(ImDrawVert);
    vk::DeviceSize indexBufferSize = imDrawData->TotalIdxCount * sizeof(ImDrawIdx);

    if ((vertexBufferSize == 0) || (indexBufferSize == 0))
    {
        return;
    }

    auto device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();
    // Update buffers only if vertex or index count has been changed compared to current buffer size
    if (!mVertexBuffer || !mVertexBuffer->mBuffer
        || (mVertexCount != imDrawData->TotalVtxCount)
    ) {
        mVertexBuffer = CreateSharedPtr<VulkanBuffer>(
            vk::BufferUsageFlagBits::eVertexBuffer,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            vertexBufferSize,
            vk::SharingMode::eExclusive);
        mVertexCount = imDrawData->TotalVtxCount;
        mVertexBuffer->Map();
    }

    if (!mIndexBuffer || !mIndexBuffer->mBuffer
        || (mIndexCount != imDrawData->TotalIdxCount)
    ) {
        mIndexBuffer = CreateSharedPtr<VulkanBuffer>(
            vk::BufferUsageFlagBits::eIndexBuffer,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            indexBufferSize,
            vk::SharingMode::eExclusive);
        mIndexCount = imDrawData->TotalIdxCount;
        mIndexBuffer->Map();
    }

    // Upload data
    ImDrawVert* vertData = (ImDrawVert*)mVertexBuffer->mMappedRegion;
    ImDrawIdx* idxData = (ImDrawIdx*)mIndexBuffer->mMappedRegion;

    for (int i = 0; i < imDrawData->CmdListsCount; ++i)
    {
        const ImDrawList* cmdList = imDrawData->CmdLists[i];
        memcpy(vertData, cmdList->VtxBuffer.Data, cmdList->VtxBuffer.Size * sizeof(ImDrawVert));
        memcpy(idxData, cmdList->IdxBuffer.Data, cmdList->IdxBuffer.Size * sizeof(ImDrawIdx));
        vertData += cmdList->VtxBuffer.Size;
        idxData += cmdList->IdxBuffer.Size;
    }
}

void VulkanImGui::DrawFrame(vk::CommandBuffer cmdBuffer)
{
    ImGuiIO& io = ImGui::GetIO();

    cmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mPipelineLayout.get(), 0, 1, &mDescriptorSets.at(0).get(), 0, nullptr);
    cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline.get());

    vk::Viewport viewport = {
        io.DisplaySize.x,   // width
        io.DisplaySize.y,   // height
        0.0f,               // minDepth
        1.0f                // maxDepth
    };
    cmdBuffer.setViewport(0, 1, &viewport);

    // UI scale and translate via push constants
    mPushConstBlock.scale = glm::vec2(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y);
    mPushConstBlock.translate = glm::vec2(-1.0f);
    cmdBuffer.pushConstants(mPipelineLayout.get(), vk::ShaderStageFlagBits::eVertex, 0, sizeof(PushConstBlock), &mPushConstBlock);

    // Render commands
    ImDrawData* imDrawData = ImGui::GetDrawData();
    int32_t vertexOffset = 0;
    int32_t indexOffset = 0;

    if (imDrawData->CmdListsCount > 0) {

        VkDeviceSize offsets[1] = { 0 };
        cmdBuffer.bindVertexBuffers(0, 1, &mVertexBuffer->Get(), offsets);
        cmdBuffer.bindIndexBuffer(mIndexBuffer->Get(), 0, vk::IndexType::eUint16);

        for (int32_t i = 0; i < imDrawData->CmdListsCount; i++)
        {
            const ImDrawList* cmd_list = imDrawData->CmdLists[i];
            for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; j++)
            {
                const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[j];
                vk::Rect2D scissorRect;
                scissorRect.offset.x = std::max((int32_t)(pcmd->ClipRect.x), 0);
                scissorRect.offset.y = std::max((int32_t)(pcmd->ClipRect.y), 0);
                scissorRect.extent.width = (uint32_t)(pcmd->ClipRect.z - pcmd->ClipRect.x);
                scissorRect.extent.height = (uint32_t)(pcmd->ClipRect.w - pcmd->ClipRect.y);
                cmdBuffer.setScissor(0, 1, &scissorRect);
                cmdBuffer.drawIndexed(pcmd->ElemCount, 1, indexOffset, vertexOffset, 0);
                indexOffset += pcmd->ElemCount;
            }
            vertexOffset += cmd_list->VtxBuffer.Size;
        }
    }
}

void VulkanImGui::InitResources()
{
    auto device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();
    ImGuiIO& io = ImGui::GetIO();

    // Create font texture
    unsigned char* fontData;
    int texWidth, texHeight;
    io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight);
    vk::DeviceSize size = texWidth * texHeight * 4 * sizeof(char);

    // Create target image for copy
    mFontImage = vk::util::CreateUniqueImage(
        texWidth, texHeight,
        vk::Format::eR8G8B8A8Unorm,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst
    );


    // Allocate memory to image
    mFontMemory = vk::util::CreateUniqueDeviceMemory(
        mFontImage.get(),
        vk::MemoryPropertyFlagBits::eDeviceLocal
    );
    device.bindImageMemory(mFontImage.get(), mFontMemory.get(), 0);

    // Create image view
    mFontImageView = vk::util::CreateUniqueImageView(
        mFontImage.get(),
        vk::Format::eR8G8B8A8Unorm,
        vk::ImageAspectFlagBits::eColor
    );

    // Staging buffer for font data upload
    vk::BufferCreateInfo bufferInfo{};
    bufferInfo.size = size;
    bufferInfo.usage = vk::BufferUsageFlagBits::eTransferSrc;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;
    vk::UniqueBuffer stagingBuffer = device.createBufferUnique(bufferInfo);

    vk::MemoryRequirements memoryReqs = device.getBufferMemoryRequirements(stagingBuffer.get());
    vk::MemoryAllocateInfo allocInfo{};
    allocInfo.allocationSize = memoryReqs.size;
    allocInfo.memoryTypeIndex = vk::util::FindMemoryType(
        memoryReqs.memoryTypeBits,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
    );

    vk::UniqueDeviceMemory stagingBufferMemory = device.allocateMemoryUnique(allocInfo);
    device.bindBufferMemory(stagingBuffer.get(), stagingBufferMemory.get(), 0);

    void* data = device.mapMemory(stagingBufferMemory.get(), 0, size, vk::MemoryMapFlags());
        memcpy(data, fontData, size);
    device.unmapMemory(stagingBufferMemory.get());

    // Prepare for transfer, switch image layout
    vk::util::SwitchImageLayout(
        mFontImage.get(),
        vk::Format::eR8G8B8A8Unorm,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eTransferDstOptimal
    );

    // Copy data from stagingBuffer to font image
    auto cmdBuffer = vk::util::BeginSingleTimeCommands();
        vk::BufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        region.imageExtent = {
            static_cast<uint32_t>(texWidth),
            static_cast<uint32_t>(texHeight),
            1
        };

        cmdBuffer.copyBufferToImage(
            stagingBuffer.get(),
            mFontImage.get(),
            vk::ImageLayout::eTransferDstOptimal,
            1,
            &region
        );
    vk::util::EndSingleTimeCommands(cmdBuffer);

    // Transition image for shader read
    vk::util::SwitchImageLayout(
        mFontImage.get(),
        vk::Format::eR8G8B8A8Unorm,
        vk::ImageLayout::eTransferDstOptimal,
        vk::ImageLayout::eShaderReadOnlyOptimal
    );

    // Create font texture sampler
    vk::SamplerCreateInfo samplerInfo{};
    samplerInfo.magFilter    = vk::Filter::eLinear;
    samplerInfo.minFilter    = vk::Filter::eLinear;
    samplerInfo.mipmapMode   = vk::SamplerMipmapMode::eLinear;
    samplerInfo.addressModeU = vk::SamplerAddressMode::eClampToEdge;
    samplerInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
    samplerInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;
    samplerInfo.borderColor  = vk::BorderColor::eFloatOpaqueWhite;

    mSampler = device.createSamplerUnique(samplerInfo);

    { // Create descriptor pool
        std::vector<vk::DescriptorPoolSize> poolSizes =
        {
            { vk::DescriptorType::eCombinedImageSampler, 1},
        };
        vk::DescriptorPoolCreateInfo descriptorPoolInfo{};
        descriptorPoolInfo.maxSets = 3;
        descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        descriptorPoolInfo.pPoolSizes = poolSizes.data();
        mDescriptorPool = device.createDescriptorPoolUnique(descriptorPoolInfo);
    }

    { // Descriptor Set Layout
        std::vector<vk::DescriptorSetLayoutBinding> layoutBindings;
        vk::DescriptorSetLayoutBinding samplerBinding;
        samplerBinding.binding = 0;
        samplerBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        samplerBinding.descriptorCount = 1;
        samplerBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;
        layoutBindings.emplace_back(samplerBinding);

        vk::DescriptorSetLayoutCreateInfo descriptorLayoutInfo{};
        descriptorLayoutInfo.bindingCount = 1;
        descriptorLayoutInfo.pBindings = layoutBindings.data();

        mDescriptorSetLayout = device.createDescriptorSetLayoutUnique(descriptorLayoutInfo);
    }

    { // Create Descriptor Set
        vk::DescriptorSetAllocateInfo setInfo;
        setInfo.descriptorPool = mDescriptorPool.get();
        setInfo.descriptorSetCount = 1;
        setInfo.pSetLayouts = &mDescriptorSetLayout.get();
        mDescriptorSets = device.allocateDescriptorSetsUnique(setInfo);

        // TODO investigate what is this used for
        // DescriptorImageInfo
        vk::DescriptorImageInfo imageInfo{};
        imageInfo.sampler = mSampler.get();
        imageInfo.imageView = mFontImageView.get();
        imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

        std::vector<vk::WriteDescriptorSet> writeDescriptorSets;
        vk::WriteDescriptorSet writeDescriptorSet{};
        writeDescriptorSet.dstSet = mDescriptorSets.at(0).get();
        writeDescriptorSet.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        writeDescriptorSet.dstBinding = 0;
        writeDescriptorSet.pImageInfo = &imageInfo;
        writeDescriptorSet.descriptorCount = 1;

        writeDescriptorSets.emplace_back(writeDescriptorSet);

        // Update descriptor sets
        device.updateDescriptorSets(
            static_cast<uint32_t>(writeDescriptorSets.size()),
            writeDescriptorSets.data(),
            0,
            nullptr
        );
    }

    // Create pipeline cache
    vk::PipelineCacheCreateInfo pipelineCacheCreateInfo = {};
    mPipelineCache = device.createPipelineCacheUnique(pipelineCacheCreateInfo);

    { // Pipeline layout
        vk::PushConstantRange pushConstantRange = {};
        pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eVertex;
        pushConstantRange.size = sizeof(vk::PushConstantRange);
        pushConstantRange.offset = 0;

        vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
        mPipelineLayout = device.createPipelineLayoutUnique(pipelineLayoutInfo);
    }

    // Graphics pipeline for UI rendering
    // Input assembly state
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateInfo = {};
    inputAssemblyStateInfo.topology = vk::PrimitiveTopology::eTriangleList;
    inputAssemblyStateInfo.primitiveRestartEnable = false;

    // Rasterization state
    vk::PipelineRasterizationStateCreateInfo rasterizationStateInfo = {};
    rasterizationStateInfo.polygonMode = vk::PolygonMode::eFill;
    rasterizationStateInfo.cullMode = vk::CullModeFlagBits::eNone;
    rasterizationStateInfo.frontFace = vk::FrontFace::eCounterClockwise;
    rasterizationStateInfo.depthClampEnable = false;
    rasterizationStateInfo.lineWidth = 1.0f;

    // Enable Blending
    vk::PipelineColorBlendAttachmentState blendAttachmentState = {};
    blendAttachmentState.blendEnable = true;
    blendAttachmentState.colorWriteMask =
        vk::ColorComponentFlagBits::eR
        | vk::ColorComponentFlagBits::eG
        | vk::ColorComponentFlagBits::eB
        | vk::ColorComponentFlagBits::eA
        ;
    blendAttachmentState.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
    blendAttachmentState.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
    blendAttachmentState.colorBlendOp = vk::BlendOp::eAdd;
    blendAttachmentState.srcAlphaBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
    blendAttachmentState.dstAlphaBlendFactor = vk::BlendFactor::eZero;
    blendAttachmentState.alphaBlendOp = vk::BlendOp::eAdd;

    // Color blend state
    vk::PipelineColorBlendStateCreateInfo colorBlendInfo = {};
    colorBlendInfo.attachmentCount = 1;
    colorBlendInfo.pAttachments = &blendAttachmentState;

    // Depth stencil state (not enabled, but still need to create)
    vk::PipelineDepthStencilStateCreateInfo depthStencilInfo = {};
    depthStencilInfo.depthTestEnable = false;
    depthStencilInfo.depthWriteEnable = false;
    depthStencilInfo.depthCompareOp = vk::CompareOp::eLessOrEqual;
    depthStencilInfo.back.compareOp = vk::CompareOp::eAlways;

    // Viewport
    vk::PipelineViewportStateCreateInfo viewportInfo = {};
    viewportInfo.viewportCount = 1;
    viewportInfo.scissorCount = 1;
    viewportInfo.flags = vk::PipelineViewportStateCreateFlags();

    // Multisample
    vk::PipelineMultisampleStateCreateInfo multisampleInfo = {};
    multisampleInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;

    // Dynamic state
    std::vector<vk::DynamicState> dynamicStateEnables = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    };
    vk::PipelineDynamicStateCreateInfo dynamicStateInfo = {};
    dynamicStateInfo.pDynamicStates = dynamicStateEnables.data();
    dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());

    // shader stages
    std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStageInfos{};

    // Create pipeline
    vk::GraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateInfo;
    pipelineCreateInfo.pRasterizationState = &rasterizationStateInfo;
    pipelineCreateInfo.pColorBlendState = &colorBlendInfo;
    pipelineCreateInfo.pMultisampleState = &multisampleInfo;
    pipelineCreateInfo.pViewportState = &viewportInfo;
    pipelineCreateInfo.pDepthStencilState = &depthStencilInfo;
    pipelineCreateInfo.pDynamicState = &dynamicStateInfo;
    pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStageInfos.size());
    pipelineCreateInfo.pStages = shaderStageInfos.data();
    pipelineCreateInfo.layout = mPipelineLayout.get();
    pipelineCreateInfo.renderPass = VulkanContext::Get()->GetPipeline()->GetVulkanRenderPass();

    // Setup vertex bindings
    vk::VertexInputBindingDescription vertexInputBinding = {};
    vertexInputBinding.binding = 0;
    vertexInputBinding.stride = sizeof(ImDrawVert);
    vertexInputBinding.inputRate = vk::VertexInputRate::eVertex;
    std::vector<vk::VertexInputBindingDescription> vertexInputBindings = {
        vertexInputBinding
    };

    // Setup vertex attributes
    std::array<vk::VertexInputAttributeDescription, 3> vertexAttributes{};

    // Position
    vertexAttributes[0].binding = 0;
    vertexAttributes[0].location = 0;
    vertexAttributes[0].format = vk::Format::eR32G32Sfloat;
    vertexAttributes[0].offset = offsetof(ImDrawVert, pos);

    // UV
    vertexAttributes[1].binding = 0;
    vertexAttributes[1].location = 1;
    vertexAttributes[1].format = vk::Format::eR32G32Sfloat;
    vertexAttributes[1].offset = offsetof(ImDrawVert, uv);

    // Color
    vertexAttributes[2].binding = 0;
    vertexAttributes[2].location = 2;
    vertexAttributes[2].format = vk::Format::eR8G8B8A8Unorm;;
    vertexAttributes[2].offset = offsetof(ImDrawVert, col);


    // Vertex input state
    vk::PipelineVertexInputStateCreateInfo vertexInputStateInfo = {};
    vertexInputStateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexInputBindings.size());
    vertexInputStateInfo.pVertexBindingDescriptions = vertexInputBindings.data();
    vertexInputStateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributes.size());
    vertexInputStateInfo.pVertexAttributeDescriptions = vertexAttributes.data();

    pipelineCreateInfo.pVertexInputState = &vertexInputStateInfo;

    VulkanShader vert(SHADERS + "/ui.vert.spv");
    VulkanShader frag(SHADERS + "/ui.frag.spv");

    // TODO refactor this to be inside VulkanShader
    vk::PipelineShaderStageCreateInfo vertShaderStageInfo;
    vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
    vertShaderStageInfo.module = vert.GetShaderModule();
    vertShaderStageInfo.pName = "main";

    vk::PipelineShaderStageCreateInfo fragShaderStageInfo;
    fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
    fragShaderStageInfo.module = frag.GetShaderModule();
    fragShaderStageInfo.pName = "main";

    shaderStageInfos[0] = vertShaderStageInfo;
    shaderStageInfos[1] = fragShaderStageInfo;

    mPipeline = device.createGraphicsPipelineUnique(
        mPipelineCache.get(),
        pipelineCreateInfo
    );
}
