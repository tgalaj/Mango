#include "SceneNode.h"

#include <glm\gtc\matrix_transform.hpp>

SceneNode::SceneNode()
    : localTransform(glm::mat4(1.0f)),
      worldTransform(glm::mat4(1.0f)),
      normalMatrix(glm::mat3(1.0f)),
      isDirty(true)
{
}

SceneNode::~SceneNode()
{
    for (auto & child : children)
    {
        //delete child;
        //child = nullptr;
        delete child;
    }
}

void SceneNode::setPosition(glm::vec3 & position)
{
    isDirty = true;
    localTransform = glm::translate(glm::mat4(1.0f), position);
}

void SceneNode::setRotation(glm::vec3 & axis, float angleDegrees)
{
    isDirty = true;
    localTransform = glm::rotate(glm::mat4(1.0f), glm::radians(angleDegrees), axis);
}

void SceneNode::setScale(glm::vec3 & scale)
{
    isDirty = true;
    localTransform = glm::scale(glm::mat4(1.0f), scale);
}

void SceneNode::addChild(SceneNode * child)
{
    //isDirty = true;
}

void SceneNode::update(glm::mat4 & parentTransform, bool dirty)
{
    dirty |= isDirty;

    if (dirty)
    {
        worldTransform = parentTransform * localTransform;
        normalMatrix   = glm::mat3(glm::transpose(glm::inverse(worldTransform)));

        isDirty = false;
    }

    for (auto & child : children)
    {
        
    }
}
