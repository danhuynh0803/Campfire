#ifndef GL_OBJECT_H
#define GL_OBJECT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

#include "ShaderOld.h"
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
    virtual void InitRenderData() = 0;
    // Ran when game state set to PLAY/PAUSE
    // transform is provided from bullet
    virtual void DrawSim(glm::mat4 model) = 0;
    // Ran when game state set to STOP
    // this is the original transform prior to the simulation
    virtual void Draw(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) = 0;

    GLuint VAO;
    // TODO should there be a default texture and shader?
    Texture texture;
    ShaderOld* shader;
    Geometry type = NONE;
};

#endif // GL_OBJECT_H
