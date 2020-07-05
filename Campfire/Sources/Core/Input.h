#ifndef INPUT_H
#define INPUT_H

#include <string>

#include "KeyCodes.h"
#include "MouseCodes.h"

class Input
{
public:
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
};

#endif // INPUT_H
