#include "mgpch.h"
#include "AssetManager.h"

#include "AssetImporter.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

namespace mango
{
    static std::map<std::filesystem::path, AssetType> s_assetExtensionMap =
    {
        { ".mango", AssetType::Scene   },
        { ".png",   AssetType::Texture },
        { ".jpg",   AssetType::Texture },
        { ".jpeg",  AssetType::Texture },
    };

    static AssetType getAssetTypeFromFileExtension(const std::filesystem::path& extension)
    {
        if (!s_assetExtensionMap.contains(extension))
        {
            MG_CORE_WARN("Could not find AssetType for {}", extension);
            return AssetType::None;
        }

        return s_assetExtensionMap.at(extension);
    }

    bool EditorAssetManager::isAssetHandleValid(AssetHandle handle) const
    {
        return handle != 0 && m_assetRegistry.find(handle) != m_assetRegistry.end();
    }

    bool EditorAssetManager::isAssetLoaded(AssetHandle handle) const
    {
        return m_loadedAssets.find(handle) != m_loadedAssets.end();
    }

    mango::AssetType EditorAssetManager::getAssetType(AssetHandle handle) const
    {
        if (!isAssetHandleValid(handle))
        {
            return AssetType::None;
        }

        return m_assetRegistry.at(handle).type;
    }

    void EditorAssetManager::importAsset(const std::filesystem::path& filepath)
    {
        AssetHandle handle; // automatically generates new handle
        AssetMetadata metadata;
        metadata.filepath = filepath;
        metadata.type = getAssetTypeFromFileExtension(filepath.extension());

        MG_CORE_ASSERT(metadata.type != AssetType::None);

        ref<Asset> asset = AssetImporter::importAsset(handle, metadata);
        if (asset)
        {
            asset->handle           = handle;
            m_loadedAssets[handle]  = asset;
            m_assetRegistry[handle] = metadata;
            serializeAssetRegistry();
        }
    }

    const AssetMetadata& EditorAssetManager::getMetadata(AssetHandle handle) const
    {
        static AssetMetadata s_nullMetadata;
        auto it = m_assetRegistry.find(handle);
        if (it == m_assetRegistry.end())
        {
            return s_nullMetadata;
        }

        return it->second;
    }

    const std::filesystem::path& EditorAssetManager::getFilePath(AssetHandle handle) const
    {
        return getMetadata(handle).filepath;
    }

    ref<Asset> EditorAssetManager::getAsset(AssetHandle handle) const
    {
        // 1. check if handle is valid
        if (!isAssetHandleValid(handle)) 
            return nullptr;

        // 2. check if asset needs load (and if so, load)
        ref<Asset> asset = nullptr;
        if (isAssetLoaded(handle))
        {
            asset = m_loadedAssets.at(handle);
        }
        else
        {
            // load asset
            const AssetMetadata& metadata = getMetadata(handle);

            asset = AssetImporter::importAsset(handle, metadata);
            if (!asset) 
            {
                // import failed
                MG_CORE_ERROR("EditorAssetManager::getAsset - asset import failed!");
            }
        }

        // 3. return asset
        return asset;
    }

    void EditorAssetManager::serializeAssetRegistry()
    {
        auto path = Project::getActiveAssetRegistryPath();

        YAML::Emitter out;
        out << YAML::BeginMap; // Root
        out << YAML::Key << "AssetRegistry" << YAML::Value;

        out << YAML::BeginSeq;
        for (const auto& [handle, metadata] : m_assetRegistry)
        {
            out << YAML::BeginMap;
            {
                out << YAML::Key << "Handle"   << YAML::Value << handle;
                out << YAML::Key << "FilePath" << YAML::Value << metadata.filepath.generic_string();
                out << YAML::Key << "Type"     << YAML::Value << assetTypeToString(metadata.type);
            }
            out << YAML::EndMap;
        }
        out << YAML::EndSeq;
        out << YAML::EndMap; // Root

        std::ofstream fout(path);
        if (fout.is_open())
        {
            fout << out.c_str();
        }

    }

    bool EditorAssetManager::deserializeAssetRegistry()
    {
        auto path = Project::getActiveAssetRegistryPath();

        YAML::Node data;
        try
        {
            data = YAML::LoadFile(path.string());
        }
        catch (YAML::ParserException e)
        {
            MG_CORE_ERROR("Failed to load project file '{}'\n    {}", path.string(), e.what());
            return false;
        }

        // Project is a mandatory node
        auto rootNode = data["AssetRegistry"];
        if (!rootNode)
            return false;

        for (const auto& node : rootNode)
        {
            AssetHandle handle =  node["Handle"].as<uint64_t>();

            auto& metadata          = m_assetRegistry[handle];
                  metadata.filepath = node["FilePath"].as<std::string>();
                  metadata.type     = assetTypeFromString(node["Type"].as<std::string>().c_str());
        }

        return true;
    }
}