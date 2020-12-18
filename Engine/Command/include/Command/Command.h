#ifndef COMMAND_H
#define COMMAND_H

#include <glm/glm.hpp>
#include <functional>

class Command
{
public:
    virtual void Execute() = 0;
    virtual void Undo() = 0;
    virtual void Redo() = 0;
};
#endif //COMMAND_H

//ImGui Command toggle value, between the new and the old, at the target reference directly.
//but the refernece might get deleted when the component or the entity is destoryed
class ImGuiFloatCommand : public Command
{
public:
    ImGuiFloatCommand(float& target , const float& previousValue, const float& currentValue) 
        : previousValue(previousValue), currentValue(currentValue),target(target){}
    void Execute() {};
    void Undo() { target = previousValue; }
    void Redo() { target = currentValue; }
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
    void Undo() { target = previousValue; }
    void Redo() { target = currentValue; }
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
    void Undo() { target = previousValue; }
    void Redo() { target = currentValue; }
private:
    glm::vec3& target;
    glm::vec3 previousValue;
    glm::vec3 currentValue;
};

class ImGuiFloat4Command : public Command
{
public:
    ImGuiFloat4Command(glm::vec4& target, const glm::vec4& previous, const glm::vec4& current) :target(target)
    {
        previousValue = previous;
        currentValue = current;
    }
    void Execute() {};
    void Undo() { target = previousValue; }
    void Redo() { target = currentValue; }
private:
    glm::vec4& target;
    glm::vec4 previousValue;
    glm::vec4 currentValue;
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
    void Undo() { target = previousValue; }
    void Redo() { target = currentValue; }
private:
    std::string& target;
    std::string previousValue;
    std::string currentValue;
};

class ImGuiBoolCommand : public Command
{
public:
    ImGuiBoolCommand(bool& target) :target(target) 
    {
        value = target;
    }
    void Execute() {};
    void Undo(){ target = !value;}
    void Redo(){ target = value;}
private:
    bool& target;
    bool value;
};

template<typename T>
class GenericCommand : public Command
{
public:
    GenericCommand(T& target, const T& previous, const T& current) :target(target)
    {
        previousValue = previous;
        currentValue = current;
    }
    void Execute() 
    {
        target = currentValue;
    }
    void Undo()
    {
        target = previousValue;
    }
    void Redo()
    {
        target = currentValue;
    }
private:
    T& target;
    T previousValue;
    T currentValue;
};

template<typename T>
class EnumCommand : public Command
{
public:
    EnumCommand(T& target, const int& previous, const int& current) :target(target)
    {
        previousValue = previous;
        currentValue = current;
    }
    void Execute() 
    {
        target = static_cast<T>(currentValue);
    };
    void Undo()
    {
        target = static_cast<T>(previousValue);
    }
    void Redo()
    {
        target = static_cast<T>(currentValue);
    }
private:
    T& target;
    int previousValue;
    int currentValue;
};

class ActionCommand : public Command
{
public:
    template <typename F, typename G>
    ActionCommand(F&& f, G&& g) : redo(std::forward<F>(f)), undo(std::forward<G>(g)){}
    void Execute(){ redo();}
    void Undo(){ undo();}
    void Redo(){ redo();}
private:
    std::function<void()> redo;
    std::function<void()> undo;
};

class SkippedExecuteActionCommand : public Command
{
public:
    template <typename F, typename G>
    SkippedExecuteActionCommand(F&& f, G&& g) : redo(std::forward<F>(f)), undo(std::forward<G>(g)) {}
    void Execute() {}
    void Undo() { undo(); }
    void Redo() { redo(); }
private:
    std::function<void()> redo;
    std::function<void()> undo;
};