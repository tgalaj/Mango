#pragma once

#include "Mango/Core/Assertions.h"

#include <filesystem>
#include <string>

namespace mango
{
    struct ProjectConfig
    {
        std::string name = "Untitled";
        
        std::string startScene;

        std::filesystem::path assetDirectory;
    };

    class Project
    {
    public:
        static const std::filesystem::path& getProjectDirectory()
        {
            MG_CORE_VERIFY(s_activeProject);
            return s_activeProject->m_projectDirectory;
        }
        
        static std::filesystem::path getAssetDirectory()
        {
            MG_CORE_VERIFY(s_activeProject);
            return getProjectDirectory() / s_activeProject->m_config.assetDirectory;
        }

        ProjectConfig& getConfig() { return s_activeProject->m_config; }

        static std::shared_ptr<Project> getActive() { return s_activeProject; }

        static std::shared_ptr<Project> createNew();
        static std::shared_ptr<Project> load(const std::filesystem::path& path);
        static bool saveActive(const std::filesystem::path& path);

    private:
        ProjectConfig m_config;
        std::filesystem::path m_projectDirectory;

        inline static std::shared_ptr<Project> s_activeProject;
    };
}