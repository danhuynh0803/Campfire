#ifndef INPUT_H
#define INPUT_H

#include <string>
#include <unordered_map>

#include "KeyCodes.h"
#include "MouseCodes.h"

class Input
{
public:
    static bool GetKey(uint32_t key);
    static bool GetKeyDown(uint32_t key);
    static bool GetKeyUp(uint32_t key);

    static bool GetKey(KeyCode key);
    static bool GetKeyDown(KeyCode key);
    static bool GetKeyUp(KeyCode key);

    static bool GetMouseButton(MouseCode button);
    static bool GetMouseButtonDown(MouseCode button);
    static bool GetMouseButtonUp(MouseCode button);

    // TODO For when we can map string names to certain input commands
    // e.g. "Fire0" for mouse button 0, etc
    // Need to map the string to the key
    static bool GetButton(const std::string& code);
    static bool GetButtonDown(const std::string& code);
    static bool GetButtonUp(const std::string& code);

    static std::pair<float, float> GetMousePosition();
    static float GetMouseX();
    static float GetMouseY();

private:
    // Using GLFW defines
    // RELEASE = 0
    // PRESS   = 1
    // REPEAT  = 2

    // The following are only used for checking ButtonDown to prevent
    // multiple presses per frame
    static uint32_t GetKeyState(uint32_t key);
    static void SetKeyState(uint32_t key, uint8_t oldState);
    static std::unordered_map<uint32_t, uint8_t> stateMap;
};

#endif // INPUT_H
