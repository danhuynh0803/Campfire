#ifndef GL_OBJECT_H
#define GL_OBJECT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

#include "Shader.h"
#include "Texture.h"

class GlObject
{
public:
    virtual void Draw(glm::vec3 color = glm::vec3(1.0f)) = 0;

    virtual void InitRenderData() = 0;

    GLuint VAO;
    Shader shader;

    std::string name = "unnamed";
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    glm::vec3 rotation = glm::vec3(0.0f);

    // TODO should there be a default texture and shader?
    Texture texture;
};

#endif
