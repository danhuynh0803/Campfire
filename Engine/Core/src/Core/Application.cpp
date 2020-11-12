#include "Core/Application.h"
#include "Core/Timer.h"
#include "ImGui/ImGuiLayer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Core/Log.h"
#include "Core/Time.h"
#include "Core/Random.h"

#include "Renderer/Renderer.h"
#include "Physics/PhysicsManager.h"
//#include "JobSystem/JobSystem.h"

Application* Application::instance = nullptr;

Application::Application(const ApplicationProps& props)
{
    //Log::Init();
    Time::Init();
    Random::Init();
    //JobSystem::Init();

    instance = this;
    window = Window::Create({props.name, props.width, props.height});
    window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

    PhysicsManager::Init();
    Renderer::Init();

    // TODO should be part of the overlay thats handled by each application instead of in the core engine
    // Imgui overlay
    imguiLayer = new ImGuiLayer();
    PushOverlay(imguiLayer);
}

Application::~Application()
{
    // TODO move to shutdown
    Renderer::Shutdown();
    PhysicsManager::Shutdown();
    Shutdown();
}

void Application::Run()
{
    OnInit();

    while (isRunning)
    {
        Time::Update();

        for (Layer* layer : layerStack)
        {
            layer->OnUpdate(static_cast<float>(Time::deltaTime));
        }

        if (enableImgui)
        {
            imguiLayer->Begin();
            for (Layer* layer : layerStack)
            {
                layer->OnImGuiRender();
            }
            imguiLayer->End();
        }

        window->OnUpdate();
    }
}

void Application::Close()
{
    isRunning = false;
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

    dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));
    dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));

    for (auto revIt = layerStack.rbegin(); revIt != layerStack.rend(); ++revIt)
    {
        (*revIt)->OnEvent(e);
        // If event is handled by this layer then don't propogate event down layerstack
        if (e.handled) { break; }
    }
}

bool Application::OnWindowResize(WindowResizeEvent& e)
{
    if (e.GetWidth() == 0 || e.GetHeight() == 0)
    {
        // Don't propagate event further
        // to avoid resizing framebuffer to 0 dimensions
        return true;
    }

    Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
    return false;
}

bool Application::OnWindowClose(WindowCloseEvent& e)
{
    //isRunning = false;
    return false;
}
