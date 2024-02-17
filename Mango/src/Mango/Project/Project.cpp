#include "mgpch.h"
#include "Project.h"
#include "ProjectSerializer.h"

#include "Mango/Core/Services.h";
#include "Mango/Core/VFI.h"
#include "Mango/Scene/SceneManager.h";
#include "Mango/Scene/SceneSerializer.h"

namespace mango
{
    ref<Project> Project::createNew(const std::string& name, const std::filesystem::path& path)
    {
        if (s_activeProject)
        {
            VFI::removeFromSearchPath(getProjectDirectory());
            VFI::removeFromSearchPath(getAssetDirectory());
        }
        
        s_activeProject = createRef<Project>();
        
        s_activeProject->m_projectDirectory = path / name;

        auto& config          = s_activeProject->getConfig();
        config.name           = name;
        config.assetDirectory = "Assets";
        config.startScene     = "Scenes/SampleScene.mango";

        auto startScenePath = std::filesystem::path(config.startScene);

        std::filesystem::create_directories(path / name / config.assetDirectory / startScenePath.parent_path());
        saveActive(path / name / (name + MG_PROJECT_EXTENSION));
        
        auto startSceneName = startScenePath.stem().string();
        auto scene          = Services::sceneManager()->createScene(startSceneName);
        
        Services::sceneManager()->setActiveScene(scene);

        SceneSerializer::serialize(scene, path / name / config.assetDirectory / config.startScene);

        return s_activeProject;
    }

    ref<Project> Project::load(const std::filesystem::path& filepath)
    {
        if (s_activeProject)
        {
            VFI::removeFromSearchPath(getProjectDirectory());
            VFI::removeFromSearchPath(getAssetDirectory());
        }

        s_activeProject = ProjectSerializer::deserialize(filepath);

        if (s_activeProject)
        {
            s_activeProject->m_projectDirectory = filepath.parent_path();
        }

        return s_activeProject;
    }

    bool Project::saveActive(const std::filesystem::path& filepath)
    {
        if (ProjectSerializer::serialize(s_activeProject, filepath))
        {
            s_activeProject->m_projectDirectory = filepath.parent_path();
            return true;
        }
        return false;
    }
}