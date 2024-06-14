#pragma once
#include "Mango/Core/Base.h"

#include <filesystem>
#include <string>

namespace mango
{
    class AssetManagerBase;
    class EditorAssetManager;
    class RuntimeAssetManager;

    struct ProjectConfig
    {
        std::string name = "Untitled";
        
        std::string startScene;

        std::filesystem::path assetDirectory;
        std::filesystem::path assetRegistryPath; // Relative to assetDirectory
        std::filesystem::path meshPath;
        std::filesystem::path materialPath;

        // not serialized
        std::filesystem::path projectDirectory;
    };

    class Project
    {
    public:
        static const std::filesystem::path& getProjectDirectory()
        {
            MG_CORE_VERIFY(s_activeProject);
            return s_activeProject->getConfig().projectDirectory;
        }
        
        static std::filesystem::path getAssetDirectory()
        {
            MG_CORE_VERIFY(s_activeProject);
            return getProjectDirectory() / s_activeProject->m_config.assetDirectory;
        }

        static std::filesystem::path getAssetRegistryPath()
        {
            MG_CORE_VERIFY(s_activeProject);
            return getAssetDirectory() / s_activeProject->m_config.assetRegistryPath;
        }

        static std::filesystem::path getMeshPath()
        {
            MG_CORE_VERIFY(s_activeProject);
            return getAssetDirectory() / s_activeProject->m_config.meshPath;
        }

        static std::filesystem::path getMaterialPath()
        {
            MG_CORE_VERIFY(s_activeProject);
            return getAssetDirectory() / s_activeProject->m_config.materialPath;
        }

        static ProjectConfig& getConfig() { return s_activeProject->m_config; }
        static ref<Project>   getActive() { return s_activeProject; }

        ref<AssetManagerBase>    getAssetManager() { return m_assetManager; }
        // Use only for the Editor!
        ref<EditorAssetManager>  getEditorAssetManager() const;
        // Use only for the Runtime!
        ref<RuntimeAssetManager> getRuntimeAssetManager() const;

        static ref<Project> createNew (const std::string&           name, const std::filesystem::path& path);
        static ref<Project> load      (const std::filesystem::path& filepath);
        static bool         saveActive(const std::filesystem::path& filepath);

    private:
        ProjectConfig m_config;
        ref<AssetManagerBase> m_assetManager;

        inline static ref<Project> s_activeProject;

    private:
        friend class ProjectSerializer;
    };
}