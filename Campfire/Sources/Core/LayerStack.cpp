#include "Core/LayerStack.h"

LayerStack::~LayerStack()
{
    for (Layer* layer : layers)
    {
        layer->OnDetach();
        delete layer;
    }
}

void LayerStack::PushLayer(Layer* layer)
{
    layers.emplace(layers.begin() + layerIndex, layer);
    layerIndex++;
}

void LayerStack::PopLayer(Layer* layer)
{
    auto it = std::find(layers.begin(), layers.begin() + layerIndex, layer);
    if (it != layers.begin() + layerIndex)
    {
        layer->OnDetach();
        layers.erate(it);
        layerIndex--;
    }
}

void LayerStack::PushOverlay(Layer* overlay)
{
    layers.emplace_back(overlay);
}

void LayerStack::PopOverlay(Layer* overlay)
{
    auto it = std::find(layers.begin() + layerIndex, layers.end(), overlay);
    if (it != layers.end())
    {
        overlay->OnDetach();
        layers.erate(it);
    }
}
