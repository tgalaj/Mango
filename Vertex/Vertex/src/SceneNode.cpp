#include "SceneNode.h"

#include <glm\gtc\matrix_transform.hpp>

SceneNode::SceneNode()
    : localTransform(glm::mat4(1.0f)),
      worldTransform(glm::mat4(1.0f)),
      isDirty(true)
{
}


SceneNode::~SceneNode()
{
    for (auto & child : children)
    {
        delete child;
        child = nullptr;
    }
}

void SceneNode::setPosition(glm::vec3 & position)
{
    isDirty = true;
    localTransform = glm::translate(localTransform, position);
}

void SceneNode::setRotation(glm::vec3 & axis, float angleDegrees)
{
    isDirty = true;
    localTransform = glm::rotate(localTransform, glm::radians(angleDegrees), axis);
}

void SceneNode::setScale(glm::vec3 & scale)
{
    isDirty = true;
    localTransform = glm::scale(localTransform, scale);
}

void SceneNode::addChild(SceneNode * child)
{
    //isDirty = true;
    children.push_back(child);
}

void SceneNode::update(glm::mat4 & parentTransform, bool dirty)
{
    dirty |= isDirty;

    if (dirty)
    {
        worldTransform = parentTransform * localTransform;
        isDirty = false;
    }

    for (auto & child : children)
    {
        child->update(worldTransform, dirty);
    }
}
