#pragma once
#include "Util/Utility.h"

namespace EventManager
{
    template<typename T>
    std::vector<std::function<void(T&)>>& GetListeners()
    {
        static std::vector<std::function<void(T&)>> listeners;
        return listeners;
    }

    template<typename T>
    void AddListener(std::function<void(T&)> callback)
    {
        GetListeners<T>().push_back(std::move(callback));
    }

    template<typename T>
    void RemoveListener(std::function<void(T&)> callback)
    {
        //need to test this
        //Do I pass the reference or the value here?
        for (auto listener : GetListeners<T>())
        {
            if (Utilty::GetAddress(listener) == Utilty::GetAddress(callback))
            {
                GetListeners<T>().erase(listener);
            }
        }
    }

    template<typename T>
    void TriggerEvent(T& event)
    {
        for (auto& listener : GetListeners<T>())
        {
            listener(event);
        }
    }
}