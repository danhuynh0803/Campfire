#ifndef GRAPHICS_CONTEXT_H
#define GRAPHICS_CONTEXT_H

#include "Core/Base.h"

class GraphicsContext
{
public:
    virtual void Init() = 0;
    virtual void SwapBuffers() = 0;

    static std::unique_ptr<GraphicsContext> Create(void* window);
};

#endif // GRAPHICS_CONTEXT_H
