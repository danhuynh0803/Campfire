#include "Core/Input.h"
#include "Core/Application.h"

#include <GLFW/glfw3.h>


//=====================================================
//--------------------- Modifiers ---------------------
//=====================================================
uint8_t Input::mod;

bool Input::GetMod(ModKeyCode modKey, bool isExact)
{
    // Check for the exact key combination
    // For example, if Shift+Ctrl pressed down
    // and we're looking only for Shift being pressed
    // then this returns false when isExact is true
    if (isExact)
    {
        return (mod == static_cast<uint8_t>(modKey));
    }
    else
    {
        return (mod & static_cast<uint8_t>(modKey));
    }
}

// Handles combination modifiers like
// Ctrl + Shift, etc
bool Input::GetMod(uint8_t comboModKey)
{
    return (mod == comboModKey);
}

void Input::SetMod(int modKey)
{
    mod = static_cast<uint8_t>(modKey);
}

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
//----------------------- UINT ------------------------
//=====================================================
bool Input::GetKey(uint32_t key)
{
    auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    auto state = glfwGetKey(window, key);

    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

// Process input only once
bool Input::GetKeyDown(uint32_t key)
{
    auto oldState = GetKeyState(key);
    auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    auto state = glfwGetKey(window, key);

    SetKeyState(key, state);

    return state == GLFW_PRESS && oldState == GLFW_RELEASE;
}

bool Input::GetKeyUp(uint32_t key)
{
    auto oldState = GetKeyState(key);
    auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    auto state = glfwGetKey(window, key);

    SetKeyState(key, state);

    return state == GLFW_RELEASE && oldState == GLFW_PRESS;
}


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
    auto oldState = GetKeyState(static_cast<uint32_t>(key));
    auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    auto state = glfwGetKey(window, static_cast<uint32_t>(key));

    SetKeyState(static_cast<uint32_t>(key), state);

    return state == GLFW_RELEASE && oldState == GLFW_PRESS;
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
    auto oldState = GetKeyState(static_cast<uint32_t>(button));
    auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    auto state = glfwGetMouseButton(window, static_cast<uint32_t>(button));

    SetKeyState(static_cast<uint32_t>(button), state);

    return state == GLFW_RELEASE && oldState == GLFW_PRESS;
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