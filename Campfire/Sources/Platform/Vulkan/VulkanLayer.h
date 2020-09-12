#pragma once

#include "Core/Layer.h"
#include "VulkanContext.h"

class VulkanLayer : public Layer
{
public:
    VulkanLayer();
    ~VulkanLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(float timestep) override;
    virtual void OnImGuiRender() override;
    virtual void OnEvent(Event& event) override;

private:
};
