#ifndef APPLICATION_H
#define APPLICATION_H

#include "Core/Window.h"
#include "Core/LayerStack.h"
#include "Events/Event.h"
#include "ImGui/ImGuiLayer.h"

class Application
{
public:
    Application();
    ~Application();

    static Application& Get() { return *instance; }
    //virtual void Start(); // Just do in constructor
    virtual void Run();
    virtual void Shutdown();

    void PushLayer(Layer* layer);
    void PushOverlay(Layer* layer);

    void OnEvent(Event& e);

    Window& GetWindow() { return *window; }

private:
    static Application* instance;
    ImGuiLayer* imguiLayer;
    bool isRunning = true;
    LayerStack layerStack;
    std::unique_ptr<Window> window;
};

static Application* CreateApplication();

#endif // APPLICATION_H
