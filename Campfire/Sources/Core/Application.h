#ifndef APPLICATION_H
#define APPLICATION_H

#include "Core/Window.h"
#include "Events/Event.h"

class Application
{
public:
    Application();
    ~Application();

    //virtual void Start(); // Just do in constructor
    virtual void Run();
    virtual void Shutdown();

    void OnEvent(Event& e);
    std::unique_ptr<Window> window;

private:
    static Application* instance;
    bool isRunning;
};

static Application* CreateApplication();

#endif // APPLICATION_H
