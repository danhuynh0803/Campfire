#pragma once

//========================================================
struct Sphere
{
    glm::vec3 pos;
    float radius;
    glm::vec3 diffuse;
    float specular;
    int id;
    glm::vec3 padding;
};

//========================================================
struct Plane
{
    glm::vec3 normal;
    float distance;
    glm::vec3 diffuse;
    float specular;
    int id;
    glm::vec3 padding;
};
