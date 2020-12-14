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
    float& target; //how should we strcut these
    float previousValue;
    float currentValue;
};

class ImGuiFloat3Command : public Command
{
public:
    ImGuiFloat3Command(glm::vec3& target, const glm::vec3& previous, const glm::vec3& current) :target(target) 
    {
        previousValue = previous;
        //mabyte use this?
        //previousValue = const_cast<glm::vec3&>(target);
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