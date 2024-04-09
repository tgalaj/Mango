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

        static ProjectConfig& getConfig() { return s_activeProject->m_config; }
        static ref<Project>   getActive() { return s_activeProject; }

        static ref<Project> createNew(const std::string& name, const std::filesystem::path& path);
        static ref<Project> load(const std::filesystem::path& filepath);
        static bool saveActive(const std::filesystem::path& filepath);

    private:
        ProjectConfig m_config;
        std::filesystem::path m_projectDirectory;

        inline static ref<Project> s_activeProject;

    private:
        friend class ProjectSerializer;
    };
}