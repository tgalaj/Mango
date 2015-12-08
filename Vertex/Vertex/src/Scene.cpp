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

void Scene::addChild(SceneNode * child, bool reflective)
{
    children.push_back(child);

    if (Model * m = dynamic_cast<Model *>(child))
    {
        if(reflective)
        {
            CoreServices::getRenderer()->addReflectiveModel(m);
        }
        else
        {
            CoreServices::getRenderer()->models.push_back(m);
        }
    }

    if (DirectionalLight * light = dynamic_cast<DirectionalLight *>(child))
    {
        CoreServices::getRenderer()->dirLights.push_back(light);
    }

    if (PointLight * light = dynamic_cast<PointLight *>(child))
    {
        CoreServices::getRenderer()->pointLights.push_back(light);
    }

    if (SpotLight * light = dynamic_cast<SpotLight *>(child))
    {
        CoreServices::getRenderer()->spotLights.push_back(light);
    }

    addChildrenToRenderer(child);
}

void Scene::addChildrenToRenderer(SceneNode * parent)
{
    for (auto & child : parent->children)
    {
        if (Model * m = dynamic_cast<Model *>(child->get()))
        {
            addChildrenToRenderer(child->get());
            CoreServices::getRenderer()->models.push_back(m);
        }

        if (DirectionalLight * light = dynamic_cast<DirectionalLight *>(child->get()))
        {
            addChildrenToRenderer(child->get());
            CoreServices::getRenderer()->dirLights.push_back(light);
        }

        if (PointLight * light = dynamic_cast<PointLight *>(child->get()))
        {
            addChildrenToRenderer(child->get());
            CoreServices::getRenderer()->pointLights.push_back(light);
        }

        if (SpotLight * light = dynamic_cast<SpotLight *>(child->get()))
        {
            addChildrenToRenderer(child->get());
            CoreServices::getRenderer()->spotLights.push_back(light);
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
