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
        std::cout << "Reloading Shader\n";
        for (auto pair : shaderMap)
        {
            // TODO Check that there was a change made to the file before reloading it
            Shader* shader = pair.second;
            Shader newShader(shader->vertexName.c_str(), shader->fragName.c_str());
            pair.second->ID = newShader.ID;
        }
    }

    void CreateShader(const char* vertPath, const char* fragPath)
    {
    }

    void Add(std::string tag, Shader* shader)
    {
        shaderMap.insert(std::pair<std::string, Shader*>(tag, shader));
    }

    Shader* Get(std::string tag)
    {
        auto it = shaderMap.find(tag);
        if (it != shaderMap.end())
        {
            return it->second;
        }
        else
        {
            std::cout << "ERROR: " << tag << " Shader is not found\n";
        }
    }


private:
    std::map<std::string, Shader*> shaderMap;
};

#endif // SHADER_CONTROLLER_H
