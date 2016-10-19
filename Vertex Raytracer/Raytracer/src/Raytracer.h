#pragma once

#include <glm/glm.hpp>

class Raytracer
{
public:
    Raytracer();
    ~Raytracer();

    glm::vec3 illuminate();

private:
    bool checkSpheresIntersections();
    bool checkTrianglesIntersections();

    void calcShadows();
    void calcLight();
    void calcReflections(); //dodać wsk na funckję illuminate

    unsigned int m_start_depth;
};
