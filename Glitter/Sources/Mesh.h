#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Texture.h"
#include "Shader.h"

#include <vector>

struct Vertex
{
    glm::vec3 position;
    glm::vec2 texCoords;
    glm::vec3 normal;
};

class Mesh
{
public:
    Mesh(std::vector<Vertex>, std::vector<GLuint>, std::vector<Texture>);

    void Draw(
        Shader* shader,
        glm::vec3 position=glm::vec3(0.0f),
        glm::vec3 rotation=glm::vec3(0.0f),
        glm::vec3 scale=glm::vec3(1.0f)
    );

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;

private:
    void InitRenderData();
    GLuint VAO, VBO, EBO;
};

#endif // MESH_H
