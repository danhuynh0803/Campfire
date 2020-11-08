#ifndef WINDOWS_WINDOW_H
#define WINDOWS_WINDOW_H

#include "Core/Window.h"
#include "Renderer/GraphicsContext.h"

#include <GLFW/glfw3.h>

// Window for Windows platform
class WindowsWindow : public Window
{
public:
    WindowsWindow(const WindowProps& props);
    virtual ~WindowsWindow();

    void OnUpdate() override;

    unsigned int GetWidth() const override { return data.width;  }
    unsigned int GetHeight() const override { return data.height; }

    // Window attributes
    void SetEventCallback(const EventCallbackFn& callback) override { data.EventCallback = callback; }
    void SetVSync(bool enabled) override;
    bool IsVSync() const override;

    virtual void* GetNativeWindow() const { return window; }

private:
    virtual void Init(const WindowProps& props);
    virtual void Shutdown();

private:
    GLFWwindow* window;
    std::unique_ptr<GraphicsContext> context;

    struct WindowData
    {
        std::string title;
        unsigned int width, height;
        bool VSync;

        EventCallbackFn EventCallback;
    };

    WindowData data;
};

#endif // WINDOWS_WINDOW_H
