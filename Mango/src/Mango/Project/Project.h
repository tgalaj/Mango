#pragma once

#include <filesystem>
#include <string>

#include "Mango/Core/Base.h"

#include "Mango/Asset/EditorAssetManager.h"
#include "Mango/Asset/RuntimeAssetManager.h"

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
        
        ref<AssetManagerBase>    getAssetManager()        { return m_assetManager; }

        // Use only for the Editor!
        ref<EditorAssetManager>  getEditorAssetManager()  { return std::static_pointer_cast<EditorAssetManager>(m_assetManager); }

        // Use only for the Runtime!
        ref<RuntimeAssetManager> getRuntimeAssetManager() { return std::static_pointer_cast<RuntimeAssetManager>(m_assetManager); }

    private:
        ProjectConfig m_config;
        std::filesystem::path m_projectDirectory;
        ref<AssetManagerBase> m_assetManager;

        inline static ref<Project> s_activeProject;

    private:
        friend class ProjectSerializer;
    };
}