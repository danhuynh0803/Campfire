#ifndef MOUSE_EVENT_H
#define MOUSE_EVENT_H

#include <sstream>
#include "Events/Event.h"
#include "Core/MouseCodes.h"

class MouseMovedEvent : public Event
{
public:
    MouseMovedEvent(float x, float y)
        : mouseX(x), mouseY(y) {}

    float GetX() const { return mouseX; }
    float GetY() const { return mouseY; }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "MouseMovedEvent: " << mouseX << ", " << mouseY;
        return ss.str();
    }

    EVENT_CLASS_TYPE(MouseMoved)
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

private:
    float mouseX, mouseY;
};

class MouseScrolledEvent : public Event
{
public:
    MouseScrolledEvent(float _xOffset, float _yOffset)
        : xOffset(_xOffset), yOffset(_yOffset) {}

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "MouseScrolledEvent: " << xOffset << ", " << yOffset;
        return ss.str();
    }

    EVENT_CLASS_TYPE(MouseScrolled)
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

    float GetXOffset() const { return xOffset; }
    float GetYOffset() const { return yOffset; }

private:
    float xOffset, yOffset;
};

class MouseButtonEvent : public Event
{
public:
    inline MouseCode GetMouseButton() const { return button; }

    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
protected:
    MouseButtonEvent(MouseCode _button)
        : button(_button) {}
    MouseCode button;
};

class MouseButtonPressedEvent : public MouseButtonEvent
{
public:
    MouseButtonPressedEvent(MouseCode button)
        : MouseButtonEvent(button) {}

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "MouseButtonPressedEvent: " << button;
        return ss.str();
    }

    EVENT_CLASS_TYPE(MouseButtonPressed)
};

class MouseButtonReleasedEvent : public MouseButtonEvent
{
public:
    MouseButtonReleasedEvent(MouseCode button)
        : MouseButtonEvent(button) {}

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "MouseButtonReleasedEvent: " << button;
        return ss.str();
    }

    EVENT_CLASS_TYPE(MouseButtonReleased);
};

#endif // MOUSE_EVENT_H
