#include "Core/Input.h"
#include "Core/Application.h"

#include <glfw/glfw3.h>

//=====================================================
//----------------------Buttons------------------------
//=====================================================

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


//=====================================================
//------------------------Keys-------------------------
//=====================================================
bool Input::GetKey(KeyCode key)
{
    auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    auto state = glfwGetKey(window, static_cast<uint32_t>(key));

    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

// Process input only once
bool Input::GetKeyDown(KeyCode key)
{
    auto oldState = GetKeyState(static_cast<uint32_t>(key));
    auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    auto state = glfwGetKey(window, static_cast<uint32_t>(key));

    SetKeyState(static_cast<uint32_t>(key), state);

    return state == GLFW_PRESS && oldState == GLFW_RELEASE;
}

bool Input::GetKeyUp(KeyCode key)
{
    auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    auto state = glfwGetKey(window, static_cast<uint32_t>(key));

    return state == GLFW_RELEASE;
}


//=====================================================
//----------------------Mouse--------------------------
//=====================================================
bool Input::GetMouseButton(MouseCode button)
{
    auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    auto state = glfwGetMouseButton(window, static_cast<uint32_t>(button));

    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Input::GetMouseButtonDown(MouseCode button)
{
    auto oldState = GetKeyState(static_cast<uint32_t>(button));
    auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    auto state = glfwGetMouseButton(window, static_cast<uint32_t>(button));

    SetKeyState(static_cast<uint32_t>(button), state);

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


//=====================================================
//--------------------Helpers--------------------------
//=====================================================
std::unordered_map<uint32_t, uint8_t> Input::stateMap;

uint32_t Input::GetKeyState(uint32_t key)
{
    auto it = stateMap.find(key);
    if (it != stateMap.end())
    {
        return it->second;
    }
    else
    {
        stateMap.emplace(key, 0);
    }
    return 0;
}

// Only used for making sure button pressed once
void Input::SetKeyState(uint32_t key, uint8_t oldState)
{
    auto it = stateMap.find(key);
    if (it != stateMap.end())
    {
        it->second = oldState;
    }
    else
    {
        stateMap.emplace(key, 0);
    }
}
