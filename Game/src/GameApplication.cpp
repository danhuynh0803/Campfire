#include "Core/Application.h"
#include "EntryPoint.h"
#include "../include/GameLayer.h"
#include "RCCpp.h"

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
        RCCpp::Init();
    }
    virtual void Update(float dt) override
    {
        RCCpp::Update(dt);
        RCCpp::systemTable.RCCppEntry->Update(dt);
    }

    virtual void Shutdown() override
    {
        RCCpp::Shutdown();
    }


    virtual void OnEvent(Event& e) override
    {
        EventDispatcher dispatcher(e);

        ////From the glfw example, this method appears to be called under all the glfw event callbacks
        //RCCpp::ResetPowerSaveCountDown();

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
