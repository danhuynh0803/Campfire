#pragma once

#include <GLFW/glfw3.h>
#include "Core/Window.h"
#include "Renderer/GraphicsContext.h"

// Window for Linux platform
class LinuxWindow : public Window
{
public:
    LinuxWindow(const WindowProps& props);
    virtual ~LinuxWindow();

    void OnUpdate() override;

    unsigned int GetWidth() const override { return data.width;  }
    unsigned int GetHeight() const override { return data.height; }

    // Window attributes
    void SetEventCallback(const EventCallbackFn& callback) override { data.EventCallback = callback; }
    void SetVSync(bool enabled) override;
    bool IsVSync() const override;

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
    };

    WindowData data;
};
