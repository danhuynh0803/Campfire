#include "Core/Application.h"
#include "EntryPoint.h"
#include "ImGui/VulkanImGuiLayer.h"
#include "../include/VulkanLayer.h"

class VulkanApplication : public Application
{
public:
    VulkanApplication(const ApplicationProps& props)
        : Application(props)
    {
        //enableImgui = false;
    }

    virtual void OnInit() override
    {
        PushLayer(new VulkanLayer());
    }
};

Application* CreateApplication()
{
    return new VulkanApplication({"Vulkan", 1920, 1080});
}
