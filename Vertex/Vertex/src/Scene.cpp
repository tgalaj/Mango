#include "Scene.h"


Scene::Scene() 
    : modelMatrix(glm::mat4(1.0f))
{
}


Scene::~Scene()
{
    for (auto & child : children)
    {
        delete child;
    }
}

/* Consider creating several addChild() methods for Models, Lights etc to easily add these objects to renderer */
void Scene::addChild(SceneNode * child)
{
    children.push_back(child);
}

void Scene::update()
{
    for (auto & child : children)
    {
        child->update(modelMatrix, false);
    }
}
