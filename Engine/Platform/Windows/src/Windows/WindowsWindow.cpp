#include <GLFW/glfw3.h>

#include "Core/Input.h"
#include "Core/Log.h"

#include "Platform/Windows/WindowsWindow.h"
#include "Platform/Windows/WindowsFileSystem.h"
#include "Renderer/RendererAPI.h"
#include "Renderer/GraphicsContext.h"

#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"

static uint8_t glfwWindowCount = 0;

WindowsWindow::WindowsWindow(const WindowProps& props)
{
    Init(props);
}

WindowsWindow::~WindowsWindow()
{
    Shutdown();
}

void WindowsWindow::Init(const WindowProps& props)
{
    data.title = props.title;
    data.width = props.width;
    data.height = props.height;

    //LOG_INFO("Creating window {0}, ({1}, {2})", props.title, props.width, props.height);

    if (glfwWindowCount == 0)
    {
        int success = glfwInit();
    }

    #if defined (DEBUG)
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    #endif

    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None:
            // LOG ERROR
            break;
        case RendererAPI::API::OpenGL:
            break;
        case RendererAPI::API::Vulkan:
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            break;
    }

    window = glfwCreateWindow(data.width, data.height, data.title.c_str(), nullptr, nullptr);
    ++glfwWindowCount;

    context = GraphicsContext::Create(window);
    context->Init();

    glfwSetWindowUserPointer(window, &data);
    SetVSync(true);

    // Setup glfw callbacks
    //glfwSetFramebufferSizeCallback(window,
    glfwSetWindowSizeCallback(window,
        [](GLFWwindow* _window, int width, int height)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(_window);
            data.width = width;
            data.height = height;

            WindowResizeEvent resizeEvent(width, height);
            data.EventCallback(resizeEvent);
        });

    glfwSetWindowCloseCallback(window,
        [](GLFWwindow* _window)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(_window);
            WindowCloseEvent closeEvent;
            data.EventCallback(closeEvent);
        });

    glfwSetWindowIconifyCallback(window,
        [](GLFWwindow* _window, int iconified)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(_window);
            WindowMinimizeEvent minimizeEvent(iconified);
            data.EventCallback(minimizeEvent);
        });

    glfwSetKeyCallback(window,
        [](GLFWwindow* _window, int key, int scancode, int action, int mods)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(_window);

            Input::SetMod(mods); // Modifier key that is pressed

            switch (action)
            {
                case GLFW_PRESS:
                {
                    KeyPressedEvent event(static_cast<KeyCode>(key), 0);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    KeyReleasedEvent event(static_cast<KeyCode>(key));
                    data.EventCallback(event);
                    break;
                }
                case GLFW_REPEAT:
                {
                    KeyPressedEvent event(static_cast<KeyCode>(key), 1); // TODO
                    data.EventCallback(event);
                    break;
                }
            }

        });

    glfwSetCharCallback(window,
        [](GLFWwindow* _window, unsigned int key)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(_window);
            KeyTypedEvent typeEvent(static_cast<KeyCode>(key));
            data.EventCallback(typeEvent);
        });

    glfwSetMouseButtonCallback(window,
        [](GLFWwindow* _window, int button, int action, int mods)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(_window);

            switch (action)
            {
                case GLFW_PRESS:
                {
                    MouseButtonPressedEvent event(static_cast<MouseCode>(button));
                    data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    MouseButtonReleasedEvent event(static_cast<MouseCode>(button));
                    data.EventCallback(event);
                    break;
                }
            }
        });

    glfwSetCursorPosCallback(window,
        [](GLFWwindow* _window, double x, double y)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(_window);
            MouseMovedEvent moveEvent((float)x, (float)y);
            data.EventCallback(moveEvent);
        });

    glfwSetScrollCallback(window,
        [](GLFWwindow* _window, double xOffset, double yOffset)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(_window);
            MouseScrolledEvent scrollEvent((float)xOffset, (float)yOffset);
            data.EventCallback(scrollEvent);
        });

    glfwSetCursorPosCallback(window,
        [](GLFWwindow* _window, double x, double y)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(_window);
            MouseMovedEvent moveEvent((float)x, (float)y);
            data.EventCallback(moveEvent);
        });

    glfwSetScrollCallback(window,
        [](GLFWwindow* _window, double xOffset, double yOffset)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(_window);
            MouseScrolledEvent scrollEvent((float)xOffset, (float)yOffset);
            data.EventCallback(scrollEvent);
        });

}

void WindowsWindow::Shutdown()
{
    glfwDestroyWindow(window);
    --glfwWindowCount;

    if (glfwWindowCount == 0)
    {
        glfwTerminate();
    }
}

void WindowsWindow::OnUpdate()
{
    glfwPollEvents();
    context->SwapBuffers();
}

void WindowsWindow::SetVSync(bool enabled)
{
    if (enabled)
    {
        glfwSwapInterval(1);
    }
    else
    {
        glfwSwapInterval(0);
    }

    data.VSync = enabled;
}

bool WindowsWindow::IsVSync() const
{
    return data.VSync;
}

void WindowsWindow::EnableCursor()
{
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void WindowsWindow::DisableCursor()
{
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}
