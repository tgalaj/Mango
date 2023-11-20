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

        std::shared_ptr<Scene>& createScene(const std::string& name);
        std::shared_ptr<Scene> loadScene(const std::filesystem::path& filepath);

        void setActiveScene(const std::shared_ptr<Scene>& scene);
        void setActiveScene(const std::string& name);

        std::shared_ptr<Scene>& getActiveScene();

    private:
        SceneManager() = default;

    private:
        std::vector<std::shared_ptr<Scene>> m_scenes; 
        std::shared_ptr<Scene> m_activeScene = nullptr;

    private:
        friend class Application;
    };
}