#include "VulkanLayer.h"
#include "VulkanContext.h"

VulkanLayer::VulkanLayer()
    : Layer("VulkanLayer")
{
}

void VulkanLayer::OnAttach()
{
    VulkanContext context;
}

void VulkanLayer::OnDetach()
{
}

void VulkanLayer::OnUpdate(float timestep)
{

}

void VulkanLayer::OnImGuiRender()
{

}

void VulkanLayer::OnEvent(Event& event)
{

}
