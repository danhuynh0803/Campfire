#include "Core/Application.h"
#include "Core/Timer.h"
#include "ImGuiLayer.h"
#include "RenderLayer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Core/Log.h"
#include "Core/Time.h"

Application* Application::instance = nullptr;

Application::Application()
{
    Log::Start();
    Time::Start();

    instance = this;
    window = Window::Create();
    window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

    // Create any necessary layers
    // Init renderer
    RenderLayer* renderLayer = new RenderLayer();
    PushLayer(renderLayer);
    // Imgui overlay
    imguiLayer = new ImGuiLayer();
    PushOverlay(imguiLayer);

}

Application::~Application()
{
    // TODO Shutdown any subsystems in the reverse order
    // of dependencies
}

void Application::Run()
{
    while (isRunning)
    {
        Time::Update();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (Layer* layer : layerStack)
        {
            layer->OnUpdate();
        }

        imguiLayer->Begin();

        // update layers in reverse
        for (Layer* layer : layerStack)
        {
            layer->OnImGuiRender();
        }
        imguiLayer->End();

        window->OnUpdate();
    }
}

void Application::Shutdown()
{

}

void Application::PushLayer(Layer* layer)
{
    layerStack.PushLayer(layer);
    layer->OnAttach();
}

void Application::PushOverlay(Layer* overlay)
{
    layerStack.PushOverlay(overlay);
    overlay->OnAttach();
}

void Application::OnEvent(Event& e)
{
    EventDispatcher dispatcher(e);

    for (auto revIt = layerStack.rbegin(); revIt != layerStack.rend(); ++revIt)
    {
        if (e.handled)
        {
            // If event is handled by this layer then dont propogate event down layerstack
            break;
        }
        (*revIt)->OnEvent(e);
    }
}
