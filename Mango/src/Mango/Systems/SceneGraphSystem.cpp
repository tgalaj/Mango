#include "mgpch.h"
#include "SceneGraphSystem.h"
#include "Mango/Core/Services.h"
#include "Mango/Scene/Scene.h"
#include "Mango/Scene/SceneManager.h"

namespace mango
{
    SceneGraphSystem::SceneGraphSystem()
        : System("SceneGraphSystem")
    {

    }

    void SceneGraphSystem::onUpdate(float dt)
    {
        MG_PROFILE_ZONE_SCOPED;
        static glm::mat4 rootTransform = glm::mat4(1.0f);

        auto activeScene = Services::sceneManager()->getActiveScene();

        if (activeScene)
        {
            auto view = activeScene->getEntitiesWithComponent<TransformComponent>();
            for (auto entity : view)
            {
                auto& tc = view.get<TransformComponent>(entity);
                tc.update(rootTransform, false); // TODO: doesn't work well as it also updates the children entities
            }
        }
    }
}
