#pragma once

#include <deque>

#include "PerspectiveCamera.h"
#include "OrthographicCamera.h"
#include "SceneNode.h"
#include "Renderer.h"

class Scene final
{
public:
    Scene(Camera * _cam);
    ~Scene();

    void addChild(SceneNode * child, bool reflective = false);
    void update();

private:
    std::deque<SceneNode *> children;
    glm::mat4 modelMatrix;

    Camera   * cam;
    Renderer * renderer;

    void addChildrenToRenderer(SceneNode * parent);
};

