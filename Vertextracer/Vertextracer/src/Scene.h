#pragma once
#include <vector>
#include <memory>

#include "Light.h"
#include "Object.h"
#include "Atmosphere.h"

class Scene
{
public:
    Scene();
    ~Scene();

    Atmosphere atmosphere;
    std::vector<Object*> m_objects;
    std::vector<Light*> m_lights;
};
