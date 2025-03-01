#pragma once

#include "Core/Window.h"
#include "Core/LayerStack.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "ImGui/ImGuiLayer.h"
#include "ImGui/VulkanImGuiLayer.h"

struct ApplicationProps
{
    std::string name;
    uint32_t width, height;
};

class Application
{
public:
    Application(const ApplicationProps& props = {"Campfire", 1600, 900});
    ~Application();

    static Application& Get() { return *instance; }
    virtual void Run();
    virtual void Close();
    virtual void Shutdown() {}
    virtual void OnInit() {}

    void PushLayer(Layer* layer);
    void PushOverlay(Layer* layer);

    virtual void OnEvent(Event& e);
    //virtual void OnLuaEvent(LuaEvent& e);

    bool OnWindowResize(WindowResizeEvent& event);
    bool OnWindowClose(WindowCloseEvent& event);
    bool OnWindowMinimize(WindowMinimizeEvent& event);
    Window& GetWindow() { return *window; }

    VulkanImGuiLayer* imguiLayer;

protected:
    bool enableImgui = true;
    static Application* instance;
private:
    bool isRunning = true;
    //ImGuiLayer* imguiLayer;
    //VulkanImGuiLayer* imguiLayer;
    LayerStack layerStack;
    std::unique_ptr<Window> window;
};

static Application* CreateApplication();
