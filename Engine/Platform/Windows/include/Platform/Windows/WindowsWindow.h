#pragma once

#include "Core/Window.h"
#include "Renderer/GraphicsContext.h"

struct GLFWwindow;

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
    void EnableCursor() override;
    void DisableCursor() override;

    virtual void* GetNativeWindow() const { return window; }

    virtual SharedPtr<GraphicsContext> GetGraphicsContext() { return context; }

private:
    virtual void Init(const WindowProps& props);
    virtual void Shutdown();

private:
    GLFWwindow* window;
    SharedPtr<GraphicsContext> context;

    struct WindowData
    {
        std::string title;
        unsigned int width, height;
        bool VSync;

        EventCallbackFn EventCallback;

        WindowData() = default;
    };

    WindowData data;
};
