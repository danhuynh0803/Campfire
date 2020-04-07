#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GlObject.h"

class Light : public GlObject
{
public:

    Light()
    {
        isLight = true;
        InitRenderData();
    }

    void Draw(glm::vec3 color = glm::vec3(1.0f))
    {
        if (!isActive) { return; }

        this->shader->use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture.ID);

        glm::mat4 model = glm::mat4(1.0f);

        // TODO translate object back to origin before rotating
        //model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        //model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        //model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

        model = glm::translate(model, position);
        model = glm::scale(model, scale);

        glUniformMatrix4fv(glGetUniformLocation(this->shader->ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

        this->shader->use();
        glUniform3fv(glGetUniformLocation(this->shader->ID, "lightColor"), 1, glm::value_ptr(static_cast<Light*>(this)->color));

        // Draw cube
        glBindVertexArray(this->VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }


    void InitRenderData()
    {
        GLfloat vertices[] = {
            // Position           // UV         // Normals
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,   0, 0, -1,
             0.5f, -0.5f, -0.5f,  1.0f, 0.0f,   0, 0, -1,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   0, 0, -1,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   0, 0, -1,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   0, 0, -1,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,   0, 0, -1,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   0, 0, 1,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   0, 0, 1,
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,   0, 0, 1,
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,   0, 0, 1,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,   0, 0, 1,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   0, 0, 1,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1, 0, 0,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  -1, 0, 0,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1, 0, 0,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1, 0, 0,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  -1, 0, 0,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1, 0, 0,

             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   1, 0, 0,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   1, 0, 0,
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   1, 0, 0,
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   1, 0, 0,
             0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   1, 0, 0,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   1, 0, 0,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   0, -1, 0,
             0.5f, -0.5f, -0.5f,  1.0f, 1.0f,   0, -1, 0,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   0, -1, 0,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   0, -1, 0,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   0, -1, 0,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   0, -1, 0,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   0, 1, 0,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   0, 1, 0,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   0, 1, 0,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   0, 1, 0,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,   0, 1, 0,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   0, 1, 0
        };

        glGenVertexArrays(1, &this->VAO);
        glBindVertexArray(this->VAO);

        GLuint VBO;
        glGenBuffers(1, &VBO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);   // position
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // texture
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float))); // normals
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
    }

    glm::vec4 color = glm::vec4(1.0f);
    float constant = 1.0f; // Should stay at 1.0f
    float linear = 0.09f;
    float quadratic = 0.032f;
};

//class DirLight : public Light
//{
//    DirLight(glm::vec3 c, glm::vec3 d) : direction(d)
//    {
//        color = c;
//    }
//
//    glm::vec3 direction;
//};
//
//class PointLight : public Light
//{
//public:
//    PointLight(glm::vec3 c, glm::vec3 p, float i) : pos(p), intensity(i)
//    {
//        color = c;
//    }
//
//    glm::vec3 pos;
//    float intensity;
//};

#endif //LIGHT_H
