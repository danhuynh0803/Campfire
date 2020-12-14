#ifndef COMMAND_H
#define COMMAND_H

#include <glm/glm.hpp>

class Command
{
public:
    virtual void Execute() = 0;
    virtual void Undo() = 0;
    virtual void Redo() = 0;
};
#endif //COMMAND_H

class ImGuiFloatCommand : public Command
{
public:
    ImGuiFloatCommand(float& target , const float& previousValue, const float& currentValue) 
        : previousValue(previousValue), currentValue(currentValue),target(target){}
    void Execute() {};
    void Undo()
    {
        target = previousValue;
    }
    void Redo()
    {
        target = currentValue;
    }
private:
    float& target;
    float previousValue;
    float currentValue;
};

class ImGuiFloat2Command : public Command
{
public:
    ImGuiFloat2Command(glm::vec2& target, const glm::vec2& previous, const glm::vec2& current) :target(target)
    {
        previousValue = previous;
        currentValue = current;
    }
    void Execute() {};
    void Undo()
    {
        target = previousValue;
    }
    void Redo()
    {
        target = currentValue;
    }
private:
    glm::vec2& target;
    glm::vec2 previousValue;
    glm::vec2 currentValue;
};

class ImGuiFloat3Command : public Command
{
public:
    ImGuiFloat3Command(glm::vec3& target, const glm::vec3& previous, const glm::vec3& current) :target(target)
    {
        previousValue = previous;
        currentValue = current;
    }
    void Execute() {};
    void Undo()
    {
        target = previousValue;
    }
    void Redo()
    {
        target = currentValue;
    }
private:
    glm::vec3& target;
    glm::vec3 previousValue;
    glm::vec3 currentValue;
};

class ImGuiStringCommand : public Command
{
public:
    ImGuiStringCommand(std::string& target, const std::string& previous, const std::string& current) :target(target)
    {
        previousValue = previous;
        currentValue = current;
    }
    void Execute() {};
    void Undo()
    {
        target = previousValue;
    }
    void Redo()
    {
        target = currentValue;
    }
private:
    std::string& target;
    std::string previousValue;
    std::string currentValue;
};

class ImGuiBoolCommand : public Command
{
public:
    ImGuiBoolCommand(bool& target, const bool& previous, const bool& current) :target(target)
    {
        previousValue = previous;
        currentValue = current;
    }
    void Execute() {};
    void Undo()
    {
        target = previousValue;
    }
    void Redo()
    {
        target = currentValue;
    }
private:
    bool& target;
    bool previousValue;
    bool currentValue;
};