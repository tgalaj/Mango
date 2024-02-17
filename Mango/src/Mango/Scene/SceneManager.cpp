#include "SceneManager.h"
#include "Mango/Core/Services.h"
#include "Mango/Scene/Scene.h"
#include "Mango/Events/SceneEvents.h"

namespace mango
{
    ref<Scene>& SceneManager::createScene(const std::string& name)
    {
        MG_PROFILE_ZONE_SCOPED;
        m_scenes.emplace_back(createRef<Scene>(name));

        // return the last created scene
        return m_scenes[m_scenes.size() - 1];
    }

    ref<Scene> SceneManager::loadScene(const std::filesystem::path& filepath)
    {
        MG_PROFILE_ZONE_SCOPED;
        // TODO: implement parsing scene from file
        return nullptr;
    }

    void SceneManager::setActiveScene(const ref<Scene>& scene)
    {
        MG_PROFILE_ZONE_SCOPED;
        m_activeScene = scene;

        Services::eventBus()->emit(ActiveSceneChangedEvent(m_activeScene.get()));
    }

    void SceneManager::setActiveScene(const std::string& name)
    {
        MG_PROFILE_ZONE_SCOPED;
        auto it = std::find_if(m_scenes.begin(), m_scenes.end(), 
                               [name](const ref<Scene>& scene)
                               {
                                  return scene->getName() == name;
                               });

        MG_CORE_ASSERT_MSG(it != m_scenes.end(), "Provided invalid scene name.");

        if (it != m_scenes.end())
        {
            m_activeScene = *it;
        }
    }

    ref<Scene>& SceneManager::getActiveScene()
    {
        return m_activeScene;
    }
}
