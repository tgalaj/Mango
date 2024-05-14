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
        std::filesystem::path assetRegistryPath; // Relative to assetDirectory
    };

    class Project
    {
    public:
        const std::filesystem::path& getProjectDirectory   ()                                  { return m_projectDirectory; }
              std::filesystem::path  getAssetDirectory     ()                                  { return getProjectDirectory() / s_activeProject->m_config.assetDirectory; }
              std::filesystem::path  getAssetRegistryPath  ()                                  { return getAssetDirectory() / s_activeProject->m_config.assetRegistryPath; }
              std::filesystem::path  getAssetFileSystemPath(const std::filesystem::path& path) { return getAssetDirectory() / path; }
              std::filesystem::path  getAssetAbsolutePath  (const std::filesystem::path& path) { return getAssetDirectory() / path; }

        static const std::filesystem::path& getActiveProjectDirectory()
        {
            MG_CORE_VERIFY(s_activeProject);
            return s_activeProject->getProjectDirectory();
        }
        
        static std::filesystem::path getActiveAssetDirectory()
        {
            MG_CORE_VERIFY(s_activeProject);
            return s_activeProject->getAssetDirectory();
        }

        static std::filesystem::path getActiveAssetRegistryPath()
        {
            MG_CORE_VERIFY(s_activeProject);
            return s_activeProject->getAssetRegistryPath();
        }

        static std::filesystem::path getActiveAssetFileSystemPath(const std::filesystem::path& path)
        {
            MG_CORE_VERIFY(s_activeProject);
            return s_activeProject->getAssetFileSystemPath(path);
        }

        static ProjectConfig& getConfig() { return s_activeProject->m_config; }
        static ref<Project>   getActive() { return s_activeProject; }

        ref<AssetManagerBase>    getAssetManager()        { return m_assetManager; }
        // Use only for the Editor!
        ref<EditorAssetManager>  getEditorAssetManager()  { return std::static_pointer_cast<EditorAssetManager>(m_assetManager); }
        // Use only for the Runtime!
        ref<RuntimeAssetManager> getRuntimeAssetManager() { return std::static_pointer_cast<RuntimeAssetManager>(m_assetManager); }

        static ref<Project> createNew (const std::string&           name, const std::filesystem::path& path);
        static ref<Project> load      (const std::filesystem::path& filepath);
        static bool         saveActive(const std::filesystem::path& filepath);

    private:
        ProjectConfig m_config;
        std::filesystem::path m_projectDirectory;
        ref<AssetManagerBase> m_assetManager;

        inline static ref<Project> s_activeProject;

    private:
        friend class ProjectSerializer;
    };
}