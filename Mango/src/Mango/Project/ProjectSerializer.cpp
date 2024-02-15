#include "mgpch.h"
#include "ProjectSerializer.h"

#include <yaml-cpp/yaml.h>

#include <fstream>

namespace mango
{
    bool ProjectSerializer::serialize(const std::shared_ptr<Project>& project, const std::filesystem::path& outFilepath)
    {
        const auto& config = project->getConfig();

        YAML::Emitter out;
        out << YAML::BeginMap;
        {
            out << YAML::Key << "Project"  << YAML::Value;
            out << YAML::BeginMap;
            {
                out << YAML::Key << "Name"  << YAML::Value << config.name;
                out << YAML::Key << "StartScene"  << YAML::Value << config.startScene;
                out << YAML::Key << "AssetDirectory"  << YAML::Value << config.assetDirectory.string();
            }
            out << YAML::EndMap;
        }
        out << YAML::EndMap;

        std::ofstream fout(outFilepath);
        if (fout.is_open())
        {
            fout << out.c_str();
            return true;
        }

        return false;
    }

    std::shared_ptr<mango::Project> ProjectSerializer::deserialize(const std::filesystem::path& inFilepath)
    {
        YAML::Node data;
        try
        {
            data = YAML::LoadFile(inFilepath.string());
        }
        catch (YAML::ParserException e)
        {
            MG_CORE_ERROR("Failed to load project file '{}'\n    {}", inFilepath.string(), e.what());
            return nullptr;
        }

        // Project is a mandatory node
        auto projectNode = data["Project"];
        if (!projectNode)
            return nullptr;

        auto project = Project::createNew();
        auto& config = project->getConfig();

        config.name           = projectNode["Name"].as<std::string>();
        config.startScene     = projectNode["StartScene"].as<std::string>();
        config.assetDirectory = projectNode["AssetDirectory"].as<std::string>();

        return project;
    }
}