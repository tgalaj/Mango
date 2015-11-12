#pragma once

#include <glm\glm.hpp>
#include <deque>

#include "watch_ptr.h"

class SceneNode : public Watchable
{
public:
    friend class Scene;

    SceneNode();
    virtual ~SceneNode();

    void setPosition(glm::vec3 & position);
    void setRotation(glm::vec3 & axis, float angleDegrees);
    void setScale   (glm::vec3 & scale);

    void addChild(SceneNode * child);
    void update(glm::mat4 & parentTransform, bool dirty);

protected:
    std::deque<SceneNode *> children;
    glm::mat4 localTransform;
    glm::mat4 worldTransform;

    bool isDirty;
};

