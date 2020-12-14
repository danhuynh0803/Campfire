#ifndef COMMAND_H
#define COMMAND_H

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