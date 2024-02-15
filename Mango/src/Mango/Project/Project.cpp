#include "mgpch.h"
#include "Project.h"
#include "ProjectSerializer.h"

namespace mango
{
    std::shared_ptr<mango::Project> Project::createNew()
    {
        s_activeProject = std::make_shared<Project>();
        return s_activeProject;
    }

    std::shared_ptr<mango::Project> Project::load(const std::filesystem::path& path)
    {
        s_activeProject = ProjectSerializer::deserialize(path);

        if (s_activeProject)
        {
            s_activeProject->m_projectDirectory = path.parent_path();
        }

        return s_activeProject;
    }

    bool Project::saveActive(const std::filesystem::path& path)
    {
        if (ProjectSerializer::serialize(s_activeProject, path))
        {
            s_activeProject->m_projectDirectory = path.parent_path();
            return true;
        }
        return false;
    }
}