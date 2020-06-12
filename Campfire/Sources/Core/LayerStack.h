#ifndef LAYER_STACK_H
#define LAYER_STACK_H

#include <vector>

#include "Core/Layer.h"

class LayerStack
{
public:
    LayerStack() = default;
    ~LayerStack();

    void PushLayer(Layer* layer);
    void PopLayer(Layer* layer);
    void PushOverlay(Layer* overlay);
    void PopOverlay(Layer* overlay);

    std::vector<Layer*>::iterator begin() { return layers.begin(); }
    std::vector<Layer*>::iterator end()   { return layers.end(); }
    std::vector<Layer*>::reverse_iterator rbegin() { return layers.rbegin(); }
    std::vector<Layer*>::reverse_iterator rend()   { return layers.rend(); }

    std::vector<Layer*>::const_iterator begin() { return layers.begin(); }
    std::vector<Layer*>::const_iterator end()   { return layers.end(); }
    std::vector<Layer*>::const_reverse_iterator rbegin() { return layers.rbegin(); }
    std::vector<Layer*>::const_reverse_iterator rend()   { return layers.rend(); }

private:
    std::vector<Layer*> layers;
    unsigned int layerIndex = 0;
};

#endif // LAYER_STACK_H
