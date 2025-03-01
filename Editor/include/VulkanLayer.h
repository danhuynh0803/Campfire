#pragma once

#include "Renderer/Buffer.h"
#include "Core/Layer.h"
#include "Core/Timer.h"
#include "Events/ApplicationEvent.h"
#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanBuffer.h"
#include "Vulkan/VulkanTexture.h"
#include "Vulkan/VulkanMesh.h"
#include "Vulkan/VulkanGraphicsPipeline.h"

#include "Widgets/HierarchyWidget.h"
#include "Widgets/InspectorWidget.h"
#include "Widgets/TransformWidget.h"

class Scene;

class VulkanLayer : public Layer
{
public:
    VulkanLayer();
    ~VulkanLayer() = default;

    virtual void OnAttach() override;
    void ResizeTexture(uint32_t width, uint32_t height);
    virtual void OnDetach() override;
    virtual void OnUpdate(float dt) override;
    virtual void OnImGuiRender() override;
    virtual void OnEvent(Event& event) override;

    bool OnWindowResize(WindowResizeEvent& e);

private:
    void SceneHierarchy(SharedPtr<Scene> scene);
    void SetupModelsScene();
    void ProcessUserInput();
    void ReconstructPipelines();

    Timer timer;
    VulkanGraphicsPipeline::TransformPushConstBlock mPushConstBlock;
    SharedPtr<Scene> scene;
    vk::Device mDevice;

    // Widgets
    HierarchyWidget mHierarchyWidget;
    InspectorWidget mInspectorWidget;
    TransformWidget mTransformWidget;
};
