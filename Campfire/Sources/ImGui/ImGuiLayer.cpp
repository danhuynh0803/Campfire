#include "Core/Application.h"
#include "ImGui/ImGuiLayer.h"
//#include "ImGui/ImGuiThemes.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

static glm::vec3 colorForText   = glm::vec3(236.f / 255.f, 240.f / 255.f, 241.f / 255.f);
static glm::vec3 colorForHeader = glm::vec3(41.f / 255.f, 128.f / 255.f, 185.f / 255.f);
static glm::vec3 colorForArea   = glm::vec3(57.f / 255.f, 79.f / 255.f, 105.f / 255.f);
static glm::vec3 colorForBody   = glm::vec3(44.f / 255.f, 62.f / 255.f, 80.f / 255.f);
static glm::vec3 colorForPops   = glm::vec3(33.f / 255.f, 46.f / 255.f, 60.f / 255.f);

static ImVec3 color_for_text = ImVec3(236.f / 255.f, 240.f / 255.f, 241.f / 255.f);
static ImVec3 color_for_head = ImVec3(41.f / 255.f, 128.f / 255.f, 185.f / 255.f);
static ImVec3 color_for_area = ImVec3(57.f / 255.f, 79.f / 255.f, 105.f / 255.f);
static ImVec3 color_for_body = ImVec3(44.f / 255.f, 62.f / 255.f, 80.f / 255.f);
static ImVec3 color_for_pops = ImVec3(33.f / 255.f, 46.f / 255.f, 60.f / 255.f);

ImGuiLayer::ImGuiLayer()
    : Layer("ImGuiLayer")
{
}

void ImGuiLayer::Theming(ImVec3 color_for_text, ImVec3 color_for_head, ImVec3 color_for_area, ImVec3 color_for_body, ImVec3 color_for_pops)
{
    ImGuiStyle& style = ImGui::GetStyle();

    style.Colors[ImGuiCol_Text] = ImVec4(color_for_text.x, color_for_text.y, color_for_text.z, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(color_for_text.x, color_for_text.y, color_for_text.z, 0.58f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(color_for_body.x, color_for_body.y, color_for_body.z, 0.95f);
    //style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 0.58f);
    style.Colors[ImGuiCol_Border] = ImVec4(color_for_body.x, color_for_body.y, color_for_body.z, 0.00f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(color_for_body.x, color_for_body.y, color_for_body.z, 0.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.78f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 0.75f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 0.47f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.21f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.78f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
    //style.Colors[ImGuiCol_ComboBg] = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 1.00f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.80f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.50f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.50f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.86f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.76f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.86f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
    //style.Colors[ImGuiCol_Column] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.32f);
    //style.Colors[ImGuiCol_ColumnHovered] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.78f);
    //style.Colors[ImGuiCol_ColumnActive] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.15f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.78f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
    //style.Colors[ImGuiCol_CloseButton] = ImVec4(color_for_text.x, color_for_text.y, color_for_text.z, 0.16f);
    //style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(color_for_text.x, color_for_text.y, color_for_text.z, 0.39f);
    //style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(color_for_text.x, color_for_text.y, color_for_text.z, 1.00f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(color_for_text.x, color_for_text.y, color_for_text.z, 0.63f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(color_for_text.x, color_for_text.y, color_for_text.z, 0.63f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.43f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(color_for_pops.x, color_for_pops.y, color_for_pops.z, 0.92f);
    style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 0.73f);
}

void ImGuiLayer::OnImGuiRender()
{
    // TODO: have a metafile to save user editor settings
    // Disable these settings for now

    // Settings to customize theme
    //ImGui::Begin("Editor Settings");
    //if (ImGui::TreeNode("Theme Settings"))
    //{
    //    ImGui::ColorEdit3("Text",   (float*)&colorForText);
    //    ImGui::ColorEdit3("Header", (float*)&colorForHeader);
    //    ImGui::ColorEdit3("Area",   (float*)&colorForArea);
    //    ImGui::ColorEdit3("Body",   (float*)&colorForBody);
    //    ImGui::ColorEdit3("Pops",   (float*)&colorForPops);

    //    color_for_text = ImVec3(colorForText.x, colorForText.y, colorForText.z);
    //    color_for_head = ImVec3(colorForHeader.x, colorForHeader.y, colorForHeader.z);
    //    color_for_area = ImVec3(colorForArea.x, colorForArea.y, colorForArea.z);
    //    color_for_body = ImVec3(colorForBody.x, colorForBody.y, colorForBody.z);
    //    color_for_pops = ImVec3(colorForPops.x, colorForPops.y, colorForPops.z);

    //    Theming(color_for_text, color_for_head, color_for_area, color_for_body, color_for_pops);

    //    ImGui::TreePop();
    //}
    //ImGui::End();
}

void ImGuiLayer::SetStopTheme()
{
    color_for_text = ImVec3(236.f / 255.f, 240.f / 255.f, 241.f / 255.f);
    color_for_head = ImVec3(41.f / 255.f, 128.f / 255.f, 185.f / 255.f);
    color_for_area = ImVec3(57.f / 255.f, 79.f / 255.f, 105.f / 255.f);
    color_for_body = ImVec3(44.f / 255.f, 62.f / 255.f, 80.f / 255.f);
    color_for_pops = ImVec3(33.f / 255.f, 46.f / 255.f, 60.f / 255.f);
    Theming(color_for_text, color_for_head, color_for_area, color_for_body, color_for_pops);
}

void ImGuiLayer::SetPlayTheme()
{
    color_for_text = ImVec3(236.f / 255.f, 240.f / 255.f, 241.f / 255.f);
    color_for_head = ImVec3(41.f / 255.f, 128.f / 255.f, 185.f / 255.f);
    color_for_area = ImVec3(57.f / 255.f, 79.f / 255.f, 105.f / 255.f);
    color_for_body = ImVec3(100.f / 255.f, 20.f / 255.f, 20.f / 255.f);
    color_for_pops = ImVec3(33.f / 255.f, 46.f / 255.f, 60.f / 255.f);
    Theming(color_for_text, color_for_head, color_for_area, color_for_body, color_for_pops);
}

void ImGuiLayer::OnAttach()
{
    // Initialize imgui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // Setup Dear ImGui style;
    ImGui::StyleColorsDark();

    SetStopTheme();

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

    // Scale font up for 4k display
    // TODO maybe have it be dynamic to match screen res or better to just have difference sized fonts since scaling causes it to be blurry

#define WIN32_CAMPFIRE_DEFAULT_HORIZONTAL_PIXEL 1920.0f

#ifdef _WIN32
    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();
    // Get the size of screen to the variable desktop
    GetWindowRect(hDesktop, &desktop);
    // The top left corner will have coordinates (0,0)
    // and the bottom right corner will have coordinates
    // (horizontal, vertical)
    //LOG_INFO("Screen Resoultion(from Windows Display): {0}x{1}",desktop.right, desktop.bottom);
    io.FontGlobalScale = desktop.right / WIN32_CAMPFIRE_DEFAULT_HORIZONTAL_PIXEL;
#else
    io.FontGlobalScale = 1.0f;
#endif
    io.FontGlobalScale = 1.0f;
}

void ImGuiLayer::OnEvent(Event& e)
{
    ImGuiIO& io = ImGui::GetIO();
    // Disable for now since viewport needs access to mouse events
    //e.handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
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
    ImGuizmo::BeginFrame();

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

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}
