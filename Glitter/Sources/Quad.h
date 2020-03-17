#ifndef QUAD_H
#define QUAD_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Quad : public GlObject
{
public:

    Quad(std::string _name, Shader _shader, glm::vec3 pos = glm::vec3(0.0f))
    {
        name.assign(_name);
        shader = _shader;
        position = pos;
        InitRenderData();
    }

    void Draw(glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 rotate = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 color = glm::vec3(1.0f))
    {
        this->shader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->texture.ID);
        glUniform1i(glGetUniformLocation(this->shader.ID, "texIn"), 0);

        glm::mat4 model = glm::mat4(1.0f);
        //model = glm::rotate(model, rotate);
        model = glm::translate(model, position);
        model = glm::scale(model, scale);
        glUniformMatrix4fv(glGetUniformLocation(this->shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(this->VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }


    void Draw(Texture &texture, glm::vec3 position, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 rotate = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 color = glm::vec3(1.0f))
    {
        this->shader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture.ID);
        glUniform1i(glGetUniformLocation(this->shader.ID, "texIn"), 0);

        glm::mat4 model = glm::mat4(1.0f);
        //model = glm::rotate(model, rotate);
        model = glm::translate(model, position);
        model = glm::scale(model, scale);
        glUniformMatrix4fv(glGetUniformLocation(this->shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(this->VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    void InitRenderData()
    {
        GLfloat vertices[] = {
            -1.0, 1.0f, 0.0f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f,
            -1.0,-1.0f, 0.0f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
             1.0,-1.0f, 0.0f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
             1.0,-1.0f, 0.0f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
             1.0, 1.0f, 0.0f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f,
            -1.0, 1.0f, 0.0f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f,
        };

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (void*)0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (void*)(5*sizeof(GLfloat)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
};

#endif // QUAD_H
