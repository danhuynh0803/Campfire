#include "Core/Input.h"
#include "Core/Application.h"

#include <glfw/glfw3.h>

//bool Input::GetButton(const std::string& code)
//{
//
//}
//
//bool Input::GetButtonDown(const std::string& code)
//{
//
//}
//
//bool Input::GetButtonUp(const std::string& code)
//{
//
//}

bool Input::GetKey(KeyCode key)
{
    auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    auto state = glfwGetKey(window, static_cast<uint32_t>(key));

    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Input::GetKeyDown(KeyCode key)
{
    auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    auto state = glfwGetKey(window, static_cast<uint32_t>(key));

    return state == GLFW_PRESS;
}

bool Input::GetKeyUp(KeyCode key)
{
    auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    auto state = glfwGetKey(window, static_cast<uint32_t>(key));

    return state == GLFW_RELEASE;
}

bool Input::GetMouseButton(MouseCode button)
{
    auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    auto state = glfwGetMouseButton(window, static_cast<uint32_t>(button));

    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Input::GetMouseButtonDown(MouseCode button)
{
    auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    auto state = glfwGetMouseButton(window, static_cast<uint32_t>(button));

    return state == GLFW_PRESS;
}

bool Input::GetMouseButtonUp(MouseCode button)
{
    auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    auto state = glfwGetMouseButton(window, static_cast<uint32_t>(button));

    return state == GLFW_RELEASE;
}

std::pair<float, float> Input::GetMousePosition()
{
    auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    return { (float)xpos, (float)ypos };
}

float Input::GetMouseX()
{
    auto[x, y] = GetMousePosition();
    return x;
}

float Input::GetMouseY()
{
    auto[x, y] = GetMousePosition();
    return y;
}
