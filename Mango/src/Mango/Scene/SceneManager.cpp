#include "SceneManager.h"
#include "Mango/Core/Services.h"
#include "Mango/Scene/Scene.h"
#include "Mango/Events/SceneEvents.h"

namespace mango
{
    std::shared_ptr<Scene>& SceneManager::createScene(const std::string& name)
    {
        MG_PROFILE_ZONE_SCOPED;
        m_scenes.emplace_back(std::make_shared<Scene>(name));

        // return the last created scene
        return m_scenes[m_scenes.size() - 1];
    }

    std::shared_ptr<Scene> SceneManager::loadScene(const std::filesystem::path& filepath)
    {
        MG_PROFILE_ZONE_SCOPED;
        // TODO: implement parsing scene from file
        return nullptr;
    }

    void SceneManager::setActiveScene(const std::shared_ptr<Scene>& scene)
    {
        MG_PROFILE_ZONE_SCOPED;
        m_activeScene = scene;

        Services::eventBus()->emit(ActiveSceneChangedEvent(m_activeScene.get()));
    }

    void SceneManager::setActiveScene(const std::string& name)
    {
        MG_PROFILE_ZONE_SCOPED;
        auto it = std::find_if(m_scenes.begin(), m_scenes.end(), 
                               [name](const std::shared_ptr<Scene>& scene)
                               {
                                  return scene->getName() == name;
                               });

        MG_CORE_ASSERT_MSG(it != m_scenes.end(), "Provided invalid scene name.");

        if (it != m_scenes.end())
        {
            m_activeScene = *it;
        }
    }

    std::shared_ptr<Scene>& SceneManager::getActiveScene()
    {
        return m_activeScene;
    }
}
