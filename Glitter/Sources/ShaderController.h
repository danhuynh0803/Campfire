#ifndef SHADER_CONTROLLER_H
#define SHADER_CONTROLLER_H

#include "Shader.h"

class ShaderController
{
public:
    void ReloadShaders()
    {
        for (Shader* shader : shaderList)
        {
            // TODO Check that there was a change made to the file before reloading it
            Shader newShader(shader->vertexName.c_str(), shader->fragName.c_str());
            shader->ID = newShader.ID;
        }
    }

    void Add(Shader* shader)
    {
        shaderList.push_back(shader);
    }

private:
    std::vector<Shader*> shaderList;
};

#endif // SHADER_CONTROLLER_H
