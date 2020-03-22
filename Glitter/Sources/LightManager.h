#ifndef LIGHT_MANAGER_H
#define LIGHT_MANAGER_H

#include <vector>

#include "GlObject.h"
#include "Light.h"

class LightManager
{
public:

    ~LightManager()
    {
        // TODO crash
        //for (auto objectPtr : objectList)
        //{
        //    delete(objectPtr);
        //}
    }

    void Add(GlObject* object)
    {
        objectList.push_back(object);
    }

    // TODO
    void Remove();

    void Draw(GLuint uboLights)
    {
        // Initialize UBOs
        int i = 0;
        for (auto light : objectList)
        {
            glm::vec3 newPos = light->position;

            //if (i == 0)
            //{
            //    float radius = 5.0f;
            //    float omega = 1.0f;

            //    newPos += radius * glm::vec3(cos(omega * glfwGetTime()),
            //            0.0f,
            //            sin(omega * glfwGetTime()));
            //}

            glBindBuffer(GL_UNIFORM_BUFFER, uboLights);

            // =======================================
            // Send in light info to light UBO
            // Position
            glBufferSubData(GL_UNIFORM_BUFFER,
                    3*sizeof(glm::vec4)*i,
                    sizeof(glm::vec4),
                    glm::value_ptr(newPos));

            // Color
            glBufferSubData(GL_UNIFORM_BUFFER,
                    3*sizeof(glm::vec4)*i + sizeof(glm::vec4),
                    sizeof(glm::vec4),
                    glm::value_ptr(static_cast<Light*>(light)->color));

            // Attenuation factors
            glm::vec4 attenuation =
                glm::vec4(
                    static_cast<Light*>(light)->constant,
                    static_cast<Light*>(light)->linear,
                    static_cast<Light*>(light)->quadratic,
                    0.0f // padding
                );

            //printf("(%f, %f, %f, %f)\n", attenuation.x, attenuation.y, attenuation.z, attenuation.w);
            glBufferSubData(GL_UNIFORM_BUFFER,
                3*sizeof(glm::vec4)*i + 2*sizeof(glm::vec4),
                sizeof(glm::vec4),
                glm::value_ptr(attenuation));
            // =======================================

            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            glBindBufferBase(GL_UNIFORM_BUFFER, 1, uboLights);

            light->shader.use();
            glUniform3fv(glGetUniformLocation(light->shader.ID, "lightColor"), 1, glm::value_ptr(static_cast<Light*>(light)->color));

            light->position = newPos;
            light->Draw();
            ++i;
        }
    }

    std::vector<GlObject*> objectList;
};

#endif // OBJECT_MANAGER_H
