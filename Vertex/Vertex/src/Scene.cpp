#include "CoreServices.h"
#include "Scene.h"


Scene::Scene(Camera * _cam)
    : modelMatrix(glm::mat4(1.0f))
{
    cam = _cam;
    CoreServices::getRenderer()->cam = _cam;
}


Scene::~Scene()
{
    for (auto & child : children)
    {
        delete child;
        child = nullptr;
    }

    delete cam;
    cam = nullptr;
}

void Scene::addChild(SceneNode * child)
{
    children.push_back(child);

    if (Model * m = dynamic_cast<Model *>(child))
    {
        CoreServices::getRenderer()->addModel(m);
    }

    addChildrenToRenderer(child);
}

void Scene::addChildrenToRenderer(SceneNode * parent)
{
    for (auto & child : parent->children)
    {
        if (Model * m = dynamic_cast<Model *>(child))
        {
            addChildrenToRenderer(child);
            CoreServices::getRenderer()->addModel(m);
        }
    }
}

void Scene::update()
{
    if (cam->isDirty)
    {
        cam->update();
    }

    for (auto & child : children)
    {
        child->update(modelMatrix, false);
    }
}
