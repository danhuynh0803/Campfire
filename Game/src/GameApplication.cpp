#include "Core/Application.h"
#include "EntryPoint.h"
#include "../include/GameLayer.h"
#include "Game.h"
#include "IObjectUtils.h"

class GameApplication : public Application
{
public:
    UniquePtr<Game> m_Game;
    GameApplication(const ApplicationProps& props)
        : Application(props)
    {
        enableImgui = false;
        instance = this;
        m_Game = std::make_unique<Game>();
    }

    virtual void OnInit() override
    {
        //PushLayer(new GameLayer());
        //RCCpp::Init();
        m_Game->Init();
        IObjectUtils::CreateUniqueObjectAndEntity("GameManager", "GameManager");
    }
    virtual void Update(float dt) override
    {
        //RCCpp::Update(dt);
        //RCCpp::systemTable.RCCppEntry->Update(dt);
        m_Game->Update(dt);
    }

    virtual void Shutdown() override
    {
        m_Game->Shutdown();
        m_Game.release();
        delete IObjectUtils::GetUniqueObject("GameManager");
        //RCCpp::systemTable.RCCppEntry->Shutdown();
        //RCCpp::Shutdown();
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
