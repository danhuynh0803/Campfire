#pragma once

#include <functional>
#include "Core/Base.h"
#include "Events/Event.h"
#include "Renderer/GraphicsContext.h"

struct WindowProps
{
    std::string title;
    unsigned int width, height;

    WindowProps(const std::string& _title = "Campfire",
                unsigned int _width = 1600,
                unsigned int _height = 900)
        : title(_title), width(_width), height(_height)
    {
    }
};

class Window
{
public:
    using EventCallbackFn = std::function<void(Event&)>;

    virtual ~Window() = default;

    virtual void OnUpdate() = 0;
    virtual unsigned int GetWidth() const = 0;
    virtual unsigned int GetHeight() const = 0;

    // Window attributes
    virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
    virtual void SetVSync(bool enabled) = 0;
    virtual bool IsVSync() const = 0;
    virtual void EnableCursor() = 0;
    virtual void DisableCursor() = 0;

    virtual void* GetNativeWindow() const = 0;

    virtual SharedPtr<GraphicsContext> GetGraphicsContext() = 0;

    static std::unique_ptr<Window> Create(const WindowProps& props = WindowProps());
};
