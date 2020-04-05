#ifndef GL_OBJECT_H
#define GL_OBJECT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

#include "Shader.h"
#include "Texture.h"

enum Geometry {
    CUBE,
    QUAD,
    SPHERE,
    LIGHT,
    MODEL,
    NONE
};

class GlObject
{
public:
    virtual void Draw(glm::vec3 color = glm::vec3(1.0f)) = 0;

    virtual void InitRenderData() = 0;

    GLuint VAO;
    Shader* shader;
    Geometry type = NONE;

    std::string name = "unnamed";
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    glm::vec3 rotation = glm::vec3(0.0f);

    glm::mat4 GetModelMatrix()
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::scale(model, scale);
        return model;
    }

    // TODO should there be a default texture and shader?
    Texture texture;

    bool isActive = true;
    bool isLight = false;
};

#endif // GL_OBJECT_H
