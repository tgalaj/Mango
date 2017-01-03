#pragma once

#include <vector>

#include "Atmosphere.h"
#include "Camera.h"
#include "Primitives.h"
#include "Light.h"

/* Responsible for parsing the input file */
class Scene
{
public:
    Scene();
    ~Scene();

    void addSphere(Sphere & s);
    void addTriangle(Triangle & t);

    void loadScene(std::string filename);

    std::vector<Sphere> spheres;
    std::vector<Triangle> triangles;

    std::vector<Light> lights;

    Atmosphere * atmosphere;
    Camera * cam;

    std::string outputFilename;
    
    int width, height, maxdepth;

private:
    bool Scene::readvals(std::stringstream &s, const int numvals, float* values) const;
    bool Scene::readvals(std::stringstream &s, const int numvals, std::string & values) const;
};

