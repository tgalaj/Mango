#pragma once
#include <vector>
#include <memory>

#include "Light.h"
#include "Model.h"
#include "Atmosphere.h"

class Scene
{
public:
    Scene();
    ~Scene();

    Atmosphere atmosphere;
    std::vector<Model*> m_objects;
    std::vector<Light*> m_lights;
};
