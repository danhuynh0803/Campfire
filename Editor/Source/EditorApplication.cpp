#include "Core/Application.h"
#include "EntryPoint.h"
//#include "EditorLayer.h"

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
    }
};

Application* CreateApplication()
{
    return new EditorApplication({"Campfire Studio", 1920, 1080});
}
