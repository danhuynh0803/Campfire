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
        instance = this;
    }

    virtual void OnInit() override
    {
        PushLayer(new GameLayer());
    }

    virtual void OnEvent(Event& e) override
    {
        EventDispatcher dispatcher(e);

        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(GameApplication::OnWindowClose));
    }

    //virtual void OnLuaEvent(LuaEvent& e) override
    //{
    //    
    //}

    bool GameApplication::OnWindowClose(WindowCloseEvent& e)
    {
        GameApplication::Get().Close();
        return false;
    }
};

Application* CreateApplication()
{
    return new GameApplication({"Game", 1920, 1080});
}
