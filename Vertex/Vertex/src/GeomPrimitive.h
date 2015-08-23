#pragma once

#include <glm\glm.hpp>
#include <vector>

#include "Model.h"

class GeomPrimitive
{
public:
    GeomPrimitive()  = delete;
    ~GeomPrimitive() = delete;

    friend class Model;

private:
    static void genCube(std::vector<glm::vec3> &positions, std::vector<glm::vec3> &normals, std::vector<glm::vec2> &texcoords, std::vector<GLubyte> &indices, float radius = 1.0f);
};

