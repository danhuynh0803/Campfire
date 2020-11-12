#ifndef SCRIPT_H
#define SCRIPT_H

class Script
{
public:
    Script(Entity entity)
        : parentEntity(entity)
    {
    }

    virtual void Start() {}
    virtual void Update() {}

private:
    Entity gameObject;
};

#endif // SCRIPT_H
