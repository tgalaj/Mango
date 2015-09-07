#pragma once

#include <deque>

#include "SceneNode.h"
#include "Renderer.h"

class Scene final
{
public:
    Scene();
    ~Scene();

    void addChild(SceneNode * child);
    void update();

private:
    std::deque<SceneNode *> children;
    glm::mat4 modelMatrix;

    Renderer * renderer;
};

