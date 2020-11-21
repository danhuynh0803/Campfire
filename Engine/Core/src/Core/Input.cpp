#include "Core/Input.h"
#include "Core/Application.h"
#include <GLFW/glfw3.h>
#include <map>

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
static const std::map<std::string, KeyCode> str2KeyMap
{
    {"KEY_SPACE"           ,Key::Space      },
    {"KEY_APOSTROPHE"      ,Key::Apostrophe }, /* ' */
    {"KEY_COMMA"           ,Key::Comma      }, /* , */
    {"KEY_MINUS"           ,Key::Minus      }, /* - */
    {"KEY_PERIOD"          ,Key::Period     }, /* . */
    {"KEY_SLASH"           ,Key::Slash      }, /* / */

    {"KEY_0"               ,Key::D0         },
    {"KEY_1"               ,Key::D1         },
    {"KEY_2"               ,Key::D2         },
    {"KEY_3"               ,Key::D3         },
    {"KEY_4"               ,Key::D4         },
    {"KEY_5"               ,Key::D5         },
    {"KEY_6"               ,Key::D6         },
    {"KEY_7"               ,Key::D7         },
    {"KEY_8"               ,Key::D8         },
    {"KEY_9"               ,Key::D9         },

    {"KEY_SEMICOLON"       ,Key::Semicolon  }, /* ; */
    {"KEY_EQUAL"           ,Key::Equal      }, /* = */

    {"KEY_A"               ,Key::A          },
    {"KEY_B"               ,Key::B          },
    {"KEY_C"               ,Key::C          },
    {"KEY_D"               ,Key::D          },
    {"KEY_E"               ,Key::E          },
    {"KEY_F"               ,Key::F          },
    {"KEY_G"               ,Key::G          },
    {"KEY_H"               ,Key::H          },
    {"KEY_I"               ,Key::I          },
    {"KEY_J"               ,Key::J          },
    {"KEY_K"               ,Key::K          },
    {"KEY_L"               ,Key::L          },
    {"KEY_M"               ,Key::M          },
    {"KEY_N"               ,Key::N          },
    {"KEY_O"               ,Key::O          },
    {"KEY_P"               ,Key::P          },
    {"KEY_Q"               ,Key::Q          },
    {"KEY_R"               ,Key::R          },
    {"KEY_S"               ,Key::S          },
    {"KEY_T"               ,Key::T          },
    {"KEY_U"               ,Key::U          },
    {"KEY_V"               ,Key::V          },
    {"KEY_W"               ,Key::W          },
    {"KEY_X"               ,Key::X          },
    {"KEY_Y"               ,Key::Y          },
    {"KEY_Z"               ,Key::Z          },

    {"KEY_LEFT_BRACKET"    ,Key::LeftBracket  }, /* [ */
    {"KEY_BACKSLASH"       ,Key::Backslash    }, /* \ */
    {"KEY_RIGHT_BRACKET"   ,Key::RightBracket }, /* ] */
    {"KEY_GRAVE_ACCENT"    ,Key::GraveAccent  }, /* ` */
    {"KEY_WORLD_1"         ,Key::World1       }, /* non-US #1 */
    {"KEY_WORLD_2"         ,Key::World2       }, /* non-US #2 */

    {"KEY_ESCAPE"          ,Key::Escape     },
    {"KEY_ENTER"           ,Key::Enter      },
    {"KEY_TAB"             ,Key::Tab        },
    {"KEY_BACKSPACE"       ,Key::Backspace  },
    {"KEY_INSERT"          ,Key::Insert     },
    {"KEY_DELETE"          ,Key::Delete     },
    {"KEY_RIGHT"           ,Key::Right      },
    {"KEY_LEFT"            ,Key::Left       },
    {"KEY_DOWN"            ,Key::Down       },
    {"KEY_UP"              ,Key::Up         },
    {"KEY_PAGE_UP"         ,Key::PageUp     },
    {"KEY_PAGE_DOWN"       ,Key::PageDown   },
    {"KEY_HOME"            ,Key::Home       },
    {"KEY_END"             ,Key::End        },
    {"KEY_CAPS_LOCK"       ,Key::CapsLock   },
    {"KEY_SCROLL_LOCK"     ,Key::ScrollLock },
    {"KEY_NUM_LOCK"        ,Key::NumLock    },
    {"KEY_PRINT_SCREEN"    ,Key::PrintScreen},
    {"KEY_PAUSE"           ,Key::Pause      },
    {"KEY_F1"              ,Key::F1         },
    {"KEY_F2"              ,Key::F2         },
    {"KEY_F3"              ,Key::F3         },
    {"KEY_F4"              ,Key::F4         },
    {"KEY_F5"              ,Key::F5         },
    {"KEY_F6"              ,Key::F6         },
    {"KEY_F7"              ,Key::F7         },
    {"KEY_F8"              ,Key::F8         },
    {"KEY_F9"              ,Key::F9         },
    {"KEY_F10"             ,Key::F10        },
    {"KEY_F11"             ,Key::F11        },
    {"KEY_F12"             ,Key::F12        },
    {"KEY_F13"             ,Key::F13        },
    {"KEY_F14"             ,Key::F14        },
    {"KEY_F15"             ,Key::F15        },
    {"KEY_F16"             ,Key::F16        },
    {"KEY_F17"             ,Key::F17        },
    {"KEY_F18"             ,Key::F18        },
    {"KEY_F19"             ,Key::F19        },
    {"KEY_F20"             ,Key::F20        },
    {"KEY_F21"             ,Key::F21        },
    {"KEY_F22"             ,Key::F22        },
    {"KEY_F23"             ,Key::F23        },
    {"KEY_F24"             ,Key::F24        },
    {"KEY_F25"             ,Key::F25        },

    {"KEY_LEFT_SHIFT"      ,Key::LeftShift      },
    {"KEY_LEFT_CONTROL"    ,Key::LeftControl    },
    {"KEY_LEFT_ALT"        ,Key::LeftAlt        },
    {"KEY_LEFT_SUPER"      ,Key::LeftSuper      },
    {"KEY_RIGHT_SHIFT"     ,Key::RightShift     },
    {"KEY_RIGHT_CONTROL"   ,Key::RightControl   },
    {"KEY_RIGHT_ALT"       ,Key::RightAlt       },
    {"KEY_RIGHT_SUPER"     ,Key::RightSuper     },
    {"KEY_MENU"            ,Key::Menu           },

};

bool Input::GetButton(const std::string& code)
{
    //KeyCode key = str2KeyMap.at(code);
    if (str2KeyMap.find(code) != str2KeyMap.end())
    {
        return Input::GetKey(str2KeyMap.at(code));
    }
    else
    {
        LOG_WARN("{0} does not have a matching KeyCode.", code);
        return false;
    }
}

bool Input::GetButtonDown(const std::string& code)
{
    //KeyCode key = str2KeyMap.at(code);
    if (str2KeyMap.find(code) != str2KeyMap.end())
    {
        return Input::GetKeyDown(str2KeyMap.at(code));
    }
    else
    {
        LOG_WARN("{0} does not have a matching KeyCode.", code);
        return false;
    }
}

bool Input::GetButtonUp(const std::string& code)
{
    //KeyCode key = str2KeyMap.at(code);
    if (str2KeyMap.find(code) != str2KeyMap.end())
    {
        return Input::GetKeyUp(str2KeyMap.at(code));
    }
    else
    {
        LOG_WARN("{0} does not have a matching KeyCode.", code);
        return false;
    }
}

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
static const std::map<std::string, MouseCode> str2MouseCodeMap
{
    { "Mouse_0", Mouse::Button0 },
    { "Mouse_1", Mouse::Button1 },
    { "Mouse_2", Mouse::Button2 },
    { "Mouse_3", Mouse::Button3 },
    { "Mouse_4", Mouse::Button4 },
    { "Mouse_5", Mouse::Button5 },
    { "Mouse_6", Mouse::Button6 },
    { "Mouse_7", Mouse::Button7 },
};

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

    return state == GLFW_PRESS && oldState == GLFW_RELEASE;
}

bool Input::GetMouseButtonUp(MouseCode button)
{
    auto oldState = GetKeyState(static_cast<uint32_t>(button));
    auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    auto state = glfwGetMouseButton(window, static_cast<uint32_t>(button));

    SetKeyState(static_cast<uint32_t>(button), state);

    return state == GLFW_RELEASE && oldState == GLFW_PRESS;
}

bool Input::GetMouseButton(const std::string& mouseCode)
{
    if (str2MouseCodeMap.find(mouseCode) != str2MouseCodeMap.end())
    {
        return Input::GetMouseButton(str2MouseCodeMap.at(mouseCode));
    }
    else
    {
        LOG_WARN("{0} does not have a matching MouseCode.", mouseCode);
        return false;
    }
}

bool Input::GetMouseButtonDown(const std::string& mouseCode)
{
    if (str2MouseCodeMap.find(mouseCode) != str2MouseCodeMap.end())
    {
        return Input::GetMouseButtonDown(str2MouseCodeMap.at(mouseCode));
    }
    else
    {
        LOG_WARN("{0} does not have a matching MouseCode.", mouseCode);
        return false;
    }
}

bool Input::GetMouseButtonUp(const std::string& mouseCode)
{
    if (str2MouseCodeMap.find(mouseCode) != str2MouseCodeMap.end())
    {
        return Input::GetMouseButtonUp(str2MouseCodeMap.at(mouseCode));
    }
    else
    {
        LOG_WARN("{0} does not have a matching MouseCode.", mouseCode);
        return false;
    }
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
