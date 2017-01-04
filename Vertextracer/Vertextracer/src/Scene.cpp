#include "Scene.h"

Scene::Scene()
{
}

Scene::~Scene()
{
    for(size_t i = 0; i < m_objects.size(); ++i)
    {
        delete m_objects[i];
    }

    m_objects.clear();

    for (size_t i = 0; i < m_lights.size(); ++i)
    {
        delete m_lights[i];
    }

    m_lights.clear();
}
