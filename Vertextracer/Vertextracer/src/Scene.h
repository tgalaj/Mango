#pragma once
#include <vector>
#include <memory>

#include "Light.h"
#include "Object.h"
#include "Atmosphere.h"

class Scene
{
public:
    Scene() {};

    Atmosphere atmosphere;
    std::vector<std::unique_ptr<Object>> m_objects;
    std::vector<std::unique_ptr<Light>> m_lights;
};
