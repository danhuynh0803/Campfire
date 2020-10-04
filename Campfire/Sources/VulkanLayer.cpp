#include "VulkanLayer.h"
#include "Platform/Vulkan/VulkanContext.h"
#include "Platform/Vulkan/VulkanRenderer.h"
#include "Core/Input.h"
#include "Scene/CameraController.h"

#include <glm/gtc/matrix_transform.hpp>

static SharedPtr<Camera> editorCamera;
static CameraController cameraController;

VulkanLayer::VulkanLayer()
    : Layer("VulkanLayer")
{
}

float vertices[] = {
    // Position           // Color            // UV
    -3.0f,  3.0f, 0.0f,   0.0f, 1.0f, 1.0f,   0.0f, 1.0f,
    -3.0f, -3.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
     3.0f, -3.0f, 0.0f,   1.0f, 0.0f, 1.0f,   1.0f, 0.0f,
     3.0f,  3.0f, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
};

uint32_t indices[] = {
    0, 1, 2,
    2, 3, 0,
};

struct CameraUBO
{
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 viewProj;
};

struct TransformUBO
{
    glm::mat4 model;
};

static CameraUBO cameraUBO;
static TransformUBO transformUBO;

void VulkanLayer::OnAttach()
{
    editorCamera = CreateSharedPtr<Camera>(1600, 900, 0.1f, 1000.0f);
    cameraController.SetActiveCamera(
        editorCamera,
        glm::vec3(0.0f, 0.0f, 10.0f), // position
        glm::vec3(0.0f, 0.0f, 0.0f) // euler angles
    );

    vertexBufferPtr = CreateSharedPtr<VulkanVertexBuffer>(vertices, sizeof(vertices));
    indexBufferPtr = CreateSharedPtr<VulkanIndexBuffer>(indices, sizeof(indices)/sizeof(uint32_t));

    auto& descriptorSets = VulkanContext::Get()->mGraphicsPipeline->descriptorSets;

    // TODO match with swapchainImages size
    // refactor to be in SetLayout
    for (size_t i = 0; i < 3; ++i)
    {
        { // Create camera UBOs
            cameraUBOs.emplace_back(CreateSharedPtr<VulkanUniformBuffer>(sizeof(CameraUBO)));

            vk::DescriptorBufferInfo bufferInfo {};
            bufferInfo.buffer = cameraUBOs[i]->GetBuffer();
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(CameraUBO);

            vk::WriteDescriptorSet descriptorWrite {};
            descriptorWrite.dstSet = descriptorSets[i].get();
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &bufferInfo;

            VulkanContext::Get()->GetDevice()->GetVulkanDevice().updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
        }

        { // Create Transform UBOs
            transformUBOs.emplace_back(CreateSharedPtr<VulkanUniformBuffer>(sizeof(TransformUBO)));

            vk::DescriptorBufferInfo bufferInfo {};
            bufferInfo.buffer = transformUBOs[i]->GetBuffer();
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(TransformUBO);

            vk::WriteDescriptorSet descriptorWrite {};
            descriptorWrite.dstSet = descriptorSets[i].get();
            descriptorWrite.dstBinding = 1;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &bufferInfo;

            VulkanContext::Get()->GetDevice()->GetVulkanDevice().updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
        }
    }
}

void VulkanLayer::OnDetach()
{
}

void VulkanLayer::OnUpdate(float dt)
{
    // TODO Create batch2d renderer
    static float rotation = 0;
    rotation += 180 * dt;
    cameraController.OnUpdate(dt);

    // Update UBO
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(rotation), glm::vec3(0, 0, 1));
    transformUBO.model = model;

    cameraUBO.view = editorCamera->GetViewMatrix();
    cameraUBO.proj = editorCamera->GetProjMatrix();
    cameraUBO.proj[1][1] *= -1;

    cameraUBO.viewProj = cameraUBO.proj * cameraUBO.view;

    auto& descriptorSets = VulkanContext::Get()->mGraphicsPipeline->descriptorSets;

    for (size_t i = 0; i < 3; ++i)
    {
        cameraUBOs[i]->SetData(&cameraUBO, 0, sizeof(CameraUBO));
        transformUBOs[i]->SetData(&transformUBO, 0, sizeof(TransformUBO));
    }

    VulkanRenderer::DrawIndexed(vertexBufferPtr->GetBuffer(), indexBufferPtr->GetBuffer(), sizeof(indices)/sizeof(uint32_t));

}

void VulkanLayer::OnImGuiRender()
{
}

void VulkanLayer::OnEvent(Event& event)
{
    cameraController.OnEvent(event);
}
