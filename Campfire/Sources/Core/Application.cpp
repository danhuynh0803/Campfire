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
#include "Editor/EditorLayer.h"
#include "Physics/PhysicsManager.h"
#include "JobSystem/JobSystem.h"
#include "../VulkanLayer.h"

Application* Application::instance = nullptr;

Application::Application()
{
    Log::Init();
    Time::Init();
    Random::Init();
    JobSystem::Init();

    instance = this;
    window = Window::Create();
    window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

    //PhysicsManager::Init();
    //Renderer::Init();

    //PushLayer(new EditorLayer());
    PushLayer(new VulkanLayer());

    // Imgui overlay
    //imguiLayer = new ImGuiLayer();
    //PushOverlay(imguiLayer);
}

Application::~Application()
{
    //Renderer::Shutdown();
    //PhysicsManager::Shutdown();
}

void Application::Run()
{
    while (isRunning)
    {
        Time::Update();

        for (Layer* layer : layerStack)
        {
            layer->OnUpdate(static_cast<float>(Time::deltaTime));
        }

        //imguiLayer->Begin();

        // update layers in reverse
        for (Layer* layer : layerStack)
        {
            //layer->OnImGuiRender();
        }
        //imguiLayer->End();

        window->OnUpdate();
    }
}

void Application::Close()
{
    isRunning = false;
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

    dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));
    dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowsClose));
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

bool Application::OnWindowResize(WindowResizeEvent& e)
{
    if (e.GetWidth() == 0 || e.GetHeight() == 0)
    {
        // TODO
        // Window minimized
        return false;
    }

    //Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
    return false;
}

bool Application::OnWindowsClose(WindowCloseEvent& e)
{
    isRunning = false;
    return false;
}
