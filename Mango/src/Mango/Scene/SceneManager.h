#pragma once

#include <filesystem>
#include <vector>

namespace mango
{
    class Scene;

    class SceneManager
    {
    public:
        virtual ~SceneManager() = default;

        ref<Scene>& createScene(const std::string& name);
        ref<Scene> loadScene(const std::filesystem::path& filepath);

        void setActiveScene(const ref<Scene>& scene);
        void setActiveScene(const std::string& name);

        ref<Scene>& getActiveScene();

    private:
        SceneManager() = default;

    private:
        std::vector<ref<Scene>> m_scenes; 
        ref<Scene>              m_activeScene = nullptr;

    private:
        friend class Application;
    };
}