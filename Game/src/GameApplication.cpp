#include "Core/Application.h"
#include "EntryPoint.h"
#include "../include/GameLayer.h"

class GameApplication : public Application
{
public:
    GameApplication(const ApplicationProps& props)
        : Application(props)
    {
        enableImgui = false;
    }

    virtual void OnInit() override
    {
        PushLayer(new GameLayer());
    }
};

Application* CreateApplication()
{
    return new GameApplication({"Game", 1920, 1080});
}
