#ifndef SHADER_CONTROLLER_H
#define SHADER_CONTROLLER_H

#include <map>

#include "Shader.h"

class ShaderController;

extern ShaderController shaderController;

class ShaderController
{
public:
    void ReloadShaders()
    {
        for (auto pair : shaderMap)
        {
            // TODO Check that there was a change made to the file before reloading it
            Shader* shader = pair.second;
            Shader newShader(shader->vertexName.c_str(), shader->fragName.c_str());
            pair.second->ID = newShader.ID;
        }
    }

    void Add(std::string tag, Shader* shader)
    {
        shaderMap.insert(std::pair<std::string, Shader*>(tag, shader));
        //shaderList.push_back(shader);
    }

    Shader* Get(std::string tag)
    {
        auto it = shaderMap.find(tag);
        if (it != shaderMap.end())
        {
            return it->second;
        }
    }


private:
    std::map<std::string, Shader*> shaderMap;
};

#endif // SHADER_CONTROLLER_H
