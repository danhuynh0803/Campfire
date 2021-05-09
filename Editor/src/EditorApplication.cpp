#include "Core/Application.h"
#include "EntryPoint.h"
#include "EditorLayer.h"
#include "VulkanLayer.h"

class EditorApplication : public Application
{
public:
    EditorApplication(const ApplicationProps& props)
        : Application(props)
    {
    }

    virtual void OnInit() override
    {
        //PushLayer(new EditorLayer);
        PushLayer(new VulkanLayer);
    }
};

Application* CreateApplication()
{
    return new EditorApplication({"Campfire Editor", 1920, 1080});
}
