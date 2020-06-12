#include "Core/Application.h"
#include "ImGuiLayer.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

ImGuiLayer::ImGuiLayer()
    : Layer("ImGuiLayer")
{
}

void ImGuiLayer::OnAttach()
{
    // Initialize imgui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // Setup Dear ImGui style;
    ImGui::StyleColorsDark();

    // Configuration flags
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    Application& app = Application::Get();
    // Setup Platform/Renderer bindings
    GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450"); // TODO set this version based on the available platform
}

void ImGuiLayer::OnEvent(Event& e)
{
    ImGuiIO& io = ImGui::GetIO();
    e.handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
    e.handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
}

void ImGuiLayer::OnDetach()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

// Beginning of frame
void ImGuiLayer::Begin()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    //ImGuizmo::BeginFrame();

    ImGui::ShowDemoWindow();
}

// End of frame
void ImGuiLayer::End()
{
    //ShowMainMenuBar();
    //ShowSceneHierarchy(*(shared.objectManager));

    ImGuiIO& io = ImGui::GetIO();
    Application& app = Application::Get();
    io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    //tentGui.ShowMetrics(timeSpan.count());
    //tentGui.RenderStateButtons(GAME);
    //tentGui.ShowCamera(camera);
    //tentGui.ShowCamera(gameCamera);
    //tentGui.ShowRenderPasses(renderPasses);
}
