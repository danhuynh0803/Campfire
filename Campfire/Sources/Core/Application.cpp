#include "Core/Application.h"
#include "Core/Timer.h"

#include <GLFW/glfw3.h>

#include <iostream>

Application* Application::instance = nullptr;

Application::Application()
{
    instance = this;
    window = Window::Create();
    window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

    // Init renderer

    // Create any necessary layers
}

Application::~Application()
{
    // TODO Shutdown any subsystems in the reverse order
    // of dependencies
}

void Application::Run()
{
    float deltaTime = 0;
    float lastFrame = 0;
    while (isRunning)
    {
        // TODO refactor profile data to display on screen
        //Timer frameTimer("Time per frame");

        // per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        window->OnUpdate();
    }
}

void Application::Shutdown()
{

}

void Application::OnEvent(Event& e)
{    
    // TODO
    //EventDispatcher dispatcher(e);
}
