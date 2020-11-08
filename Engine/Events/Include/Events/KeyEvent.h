#ifndef KEY_EVENT_H
#define KEY_EVENT_H

#include "Events/Event.h"
#include "Core/KeyCodes.h"

class KeyEvent : public Event
{
public:
    KeyCode GetKeyCode() const { return keyCode; }

    EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryKeyboard)

protected:
    KeyEvent(KeyCode keycode)
        : keyCode(keycode) {}

    KeyCode keyCode;
};

class KeyPressedEvent : public KeyEvent
{
public:
    KeyPressedEvent(KeyCode keycode, int repCount)
        : KeyEvent(keycode), repeatCount(repCount) {}

    int GetRepeatCount() const { return repeatCount; }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "KeyPressedEvent: " << keyCode << " (" << repeatCount << " repeats)";
        return ss.str();
    }

    EVENT_CLASS_TYPE(KeyPressed)

private:
    int repeatCount;
};

class KeyReleasedEvent : public KeyEvent
{
public:
    KeyReleasedEvent(KeyCode keycode)
        : KeyEvent(keycode) {}

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "KeyReleasedEvent: " << keyCode;
        return ss.str();
    }

    EVENT_CLASS_TYPE(KeyReleased)
};

class KeyTypedEvent : public KeyEvent
{
public:
    KeyTypedEvent(KeyCode keycode)
        : KeyEvent(keycode) {}

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "KeyTypedEvent: " << keyCode;
        return ss.str();
    }

    EVENT_CLASS_TYPE(KeyTyped)
};

#endif // KEY_EVENT_H
