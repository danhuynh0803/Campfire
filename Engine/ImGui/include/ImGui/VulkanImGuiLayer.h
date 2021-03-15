#pragma once

#include "Core/Layer.h"
#include "ImGui/VulkanImGui.h"

class VulkanImGuiLayer : public Layer
{
public:
    VulkanImGuiLayer();
    ~VulkanImGuiLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnEvent(Event& event) override;
    virtual void OnImGuiRender() override;

    void Begin(); // Start of frame
    void End();   // End of frame

private:
    VulkanImGuiImpl* mImGuiImpl;
};
