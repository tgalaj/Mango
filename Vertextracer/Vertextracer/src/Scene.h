#pragma once
#include <vector>

#include "Light.h"
#include "Model.h"
#include "Atmosphere.h"
#include "Options.h"

class Scene
{
public:
    Scene();
    ~Scene();

    void loadScene(const std::string & scene_file_name, Options & options);

    Atmosphere * atmosphere = nullptr;
    std::vector<Model*> m_objects;
    std::vector<Light*> m_lights;

private:
    bool Scene::readvals(std::stringstream &s, const int numvals, double* values) const;
    bool Scene::readvals(std::stringstream &s, const int numvals, std::string & values) const;
};
