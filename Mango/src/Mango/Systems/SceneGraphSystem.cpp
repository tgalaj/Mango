﻿#include "mgpch.h"
#include "SceneGraphSystem.h"

namespace mango
{
    TransformComponent SceneGraphSystem::ROOT_NODE;

    void SceneGraphSystem::onUpdate(float dt)
    {
        ROOT_NODE.update(ROOT_NODE.getWorldMatrix(), false);
    }
}
