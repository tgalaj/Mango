#include "core_components/TransformComponent.h"
#include "core_systems/SceneGraphSystem.h"
#include "core_engine/GameObject.h"
#include "core_engine/CoreServices.h"

namespace Vertex
{
    GameObject::GameObject()
    {
        entity = CoreServices::getCore()->entities.create();
        addComponent<TransformComponent>();

        SceneGraphSystem::M_ROOT_NODE.addChild(entity.component<TransformComponent>());
    }
}
