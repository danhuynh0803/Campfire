#include "VulkanLayer.h"

#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanRenderer.h"
#include "Vulkan/VulkanTexture.h"
#include "Vulkan/VulkanUtil.h"

#include "Core/Input.h"
#include "Core/ResourceManager.h" // ASSETS dir
#include "Scene/CameraController.h"

#include <glm/gtc/matrix_transform.hpp>

static SharedPtr<Camera> editorCamera;
static CameraController cameraController;

struct CameraUBO
{
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 viewProj;
};
static CameraUBO cameraUBO;

struct TransformUBO
{
    glm::mat4 model;
};
static TransformUBO transformUBO;


float vertices[] = {
    // Position           // Color            // UV
    -1.0f,  1.0f,  1.0f,   0.0f, 1.0f, 1.0f,   0.0f, 1.0f,
    -1.0f, -1.0f,  1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
     1.0f, -1.0f,  1.0f,   1.0f, 0.0f, 1.0f,   1.0f, 0.0f,
     1.0f,  1.0f,  1.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,

     1.0f,  1.0f,  1.0f,   0.0f, 1.0f, 1.0f,   0.0f, 1.0f,
     1.0f, -1.0f,  1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
     1.0f, -1.0f, -1.0f,   1.0f, 0.0f, 1.0f,   1.0f, 0.0f,
     1.0f,  1.0f, -1.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,

     1.0f,  1.0f, -1.0f,   0.0f, 1.0f, 1.0f,   0.0f, 1.0f,
     1.0f, -1.0f, -1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
    -1.0f, -1.0f, -1.0f,   1.0f, 0.0f, 1.0f,   1.0f, 0.0f,
    -1.0f,  1.0f, -1.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,

    -1.0f,  1.0f, -1.0f,   0.0f, 1.0f, 1.0f,   0.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
    -1.0f, -1.0f,  1.0f,   1.0f, 0.0f, 1.0f,   1.0f, 0.0f,
    -1.0f,  1.0f,  1.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,

    -1.0f,  1.0f, -1.0f,   0.0f, 1.0f, 1.0f,   0.0f, 1.0f,
    -1.0f,  1.0f,  1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
     1.0f,  1.0f,  1.0f,   1.0f, 0.0f, 1.0f,   1.0f, 0.0f,
     1.0f,  1.0f, -1.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,

    -1.0f, -1.0f,  1.0f,   0.0f, 1.0f, 1.0f,   0.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
     1.0f, -1.0f, -1.0f,   1.0f, 0.0f, 1.0f,   1.0f, 0.0f,
     1.0f, -1.0f,  1.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
};

uint32_t indices[] = {
    0, 1, 2,
    2, 3, 0,

    4, 5, 6,
    6, 7, 4,

    8, 9, 10,
    10, 11, 8,

    12, 13, 14,
    14, 15, 12,

    16, 17, 18,
    18, 19, 16,

    20, 21, 22,
    22, 23, 20,
};


VulkanLayer::VulkanLayer()
    : Layer("VulkanLayer")
{
}

void VulkanLayer::OnAttach()
{
    editorCamera = CreateSharedPtr<Camera>(1600, 900, 0.1f, 1000.0f);
    cameraController.SetActiveCamera(
        editorCamera,
        glm::vec3(0.0f, 0.0f, 10.0f), // position
        glm::vec3(0.0f, 0.0f, 0.0f) // euler angles
    );

    //vertexBufferPtr = CreateSharedPtr<VulkanVertexBuffer>(vertices, sizeof(vertices));
    VulkanBuffer stagingBuffer(
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        sizeof(vertices));

    auto dataRegion = stagingBuffer.Map();
        memcpy(dataRegion, vertices, sizeof(vertices));
    stagingBuffer.Unmap();

    pVertexBuffer = CreateSharedPtr<VulkanBuffer>(
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        sizeof(vertices));

    auto commandBuffer = vk::util::BeginSingleTimeCommands();
        vk::BufferCopy region;
        region.srcOffset = 0;
        region.dstOffset = 0;
        region.size = sizeof(vertices);
        commandBuffer.copyBuffer(stagingBuffer.Get(), pVertexBuffer->Get(), 1, &region);
    vk::util::EndSingleTimeCommands(commandBuffer);

    indexBufferPtr = CreateSharedPtr<VulkanIndexBuffer>(indices, sizeof(indices)/sizeof(uint32_t));

    auto& descriptorSets = VulkanContext::Get()->GetPipeline()->descriptorSets;

    for (size_t i = 0; i < 3; ++i)
    {
        textures.emplace_back(CreateSharedPtr<VulkanTexture2D>(ASSETS + "/Textures/awesomeface.png"));
    }

    // TODO match with swapchainImages size
    for (size_t i = 0; i < 3; ++i)
    {
        { // Create camera UBOs
            cameraUBOs.emplace_back(
                CreateSharedPtr<VulkanUniformBuffer>(
                    sizeof(CameraUBO),
                    descriptorSets[i].get()
                )
            );
            BufferLayout cameraLayout =
            {
                { ShaderDataType::MAT4, "view" },
                { ShaderDataType::MAT4, "proj" },
                { ShaderDataType::MAT4, "viewProj" },
            };
            cameraUBOs[i]->SetLayout(cameraLayout, 0);
        }

        { // Create Transform UBOs
            transformUBOs.emplace_back(
                CreateSharedPtr<VulkanUniformBuffer>(
                    sizeof(TransformUBO),
                    descriptorSets[i].get()
                )
            );
            BufferLayout transformLayout =
            {
                { ShaderDataType::MAT4, "model" },
            };
            transformUBOs[i]->SetLayout(transformLayout, 1);
        }

        { // Submit texture data to descriptorSet
            vk::DescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            imageInfo.imageView = textures[i]->GetImageView();
            imageInfo.sampler = textures[i]->GetSampler();

            vk::WriteDescriptorSet descriptorWrite {};
            descriptorWrite.dstSet = descriptorSets[i].get();
            descriptorWrite.dstBinding = 2;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = vk::DescriptorType::eCombinedImageSampler;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pImageInfo = &imageInfo;

            VulkanContext::Get()->GetDevice()->GetVulkanDevice().updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
        }
    }
}

void VulkanLayer::OnDetach()
{
}

void VulkanLayer::OnUpdate(float dt)
{
    // Current frame in flight
    auto frameIdx = VulkanContext::Get()->GetSwapChain()->GetCurrentImageIndex();

    // Update camera
    cameraController.OnUpdate(dt);

    // Update transform ubo
    static float rotation = 0;
    rotation += 180 * dt;
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(rotation), glm::vec3(0, 0, 1));
    transformUBO.model = model;
    transformUBOs[frameIdx]->SetData(&transformUBO, 0, sizeof(TransformUBO));

    // Update camera ubo
    cameraUBO.view = editorCamera->GetViewMatrix();
    cameraUBO.proj = editorCamera->GetProjMatrix();
    cameraUBO.proj[1][1] *= -1;
    cameraUBO.viewProj = cameraUBO.proj * cameraUBO.view;
    cameraUBOs[frameIdx]->SetData(&cameraUBO, 0, sizeof(CameraUBO));

    VulkanImGuiLayer* vkImguiLayer = Application::Get().imguiLayer;
    vkImguiLayer->Begin();
    vkImguiLayer->mImGuiImpl->UpdateBuffers();

    auto commandBuffer = VulkanRenderer::BeginScene();
        VulkanRenderer::DrawIndexed(
            commandBuffer,
            pVertexBuffer->Get(),
            indexBufferPtr->GetBuffer(),
            sizeof(indices)/sizeof(uint32_t)
        );
        vkImguiLayer->mImGuiImpl->DrawFrame(commandBuffer);
    VulkanRenderer::EndScene(commandBuffer);
}

void VulkanLayer::OnImGuiRender()
{

}

void VulkanLayer::OnEvent(Event& event)
{
    cameraController.OnEvent(event);

    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(VulkanLayer::OnWindowResize));
}

bool VulkanLayer::OnWindowResize(WindowResizeEvent& e)
{
    editorCamera->width = e.GetWidth();
    editorCamera->height = e.GetHeight();
    editorCamera->SetProjection();

    VulkanContext::Get()->RecreateSwapChain();

    return false;
}
