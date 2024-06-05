#include "mgpch.h"
#include "EditorAssetManager.h"
#include "Mango/Asset/AssetExtensions.h"
#include "Mango/Asset/AssetManager.h"
#include "Mango/Asset/AssetMetadata.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

namespace mango
{
    namespace 
    {
        static AssetMetadata s_nullMetadata;
    };

    EditorAssetManager::EditorAssetManager()
    {
        AssetImporter::init();
    }

    bool EditorAssetManager::isAssetHandleValid(AssetHandle handle) const
    {
        return handle != 0 && m_assetRegistry.contains(handle);
    }

    bool EditorAssetManager::isAssetLoaded(AssetHandle handle) const
    {
        return m_loadedAssets.contains(handle);
    }

    bool EditorAssetManager::isMemoryOnlyAsset(AssetHandle handle) const
    {
        return m_memoryAssets.contains(handle);
    }

    AssetType EditorAssetManager::getAssetType(AssetHandle handle) const
    {
        if (!isAssetHandleValid(handle))
        {
            return AssetType::None;
        }

        return m_assetRegistry.get(handle).type;
    }

    void EditorAssetManager::addMemoryOnlyAsset(const ref<Asset>& asset, const std::string& assetName /*= ""*/)
    {
        AssetMetadata metadata;
        metadata.isMemoryOnlyAsset = true;
        metadata.filepath          = assetName;
        metadata.type              = asset->getAssetType();

        m_assetRegistry[asset->assetHandle] = metadata;
        m_memoryAssets [asset->assetHandle] = asset;
    }

    void EditorAssetManager::removeAsset(AssetHandle handle)
    {
        if (m_loadedAssets.contains(handle))
            m_loadedAssets.erase(handle);

        if (m_memoryAssets.contains(handle))
            m_memoryAssets.erase(handle);

        if (m_assetRegistry.contains(handle))
            m_assetRegistry.remove(handle);

        serializeAssetRegistry();
    }

    std::unordered_set<AssetHandle> EditorAssetManager::getAllAssetsOfType(AssetType type) const
    {
        std::unordered_set<AssetHandle> result;

        for (const auto& [handle, metadata] : m_assetRegistry)
        {
            if (metadata.type == type)
            {
                result.insert(handle);
            }
        }

        return result;
    }

    const AssetMap& EditorAssetManager::getLoadedAssets() const
    {
        return m_loadedAssets;
    }

    const AssetMap& EditorAssetManager::getMemoryOnlyAssets() const
    {
        return m_memoryAssets;
    }

    AssetHandle EditorAssetManager::importAsset(const std::filesystem::path& filepath)
    {
        AssetHandle handle = getAssetHandleByFilePath(filepath.string());

        if (isAssetHandleValid(handle)) 
            return handle;

                      handle = AssetHandle{}; // generate new handle
        AssetMetadata metadata;
                      metadata.filepath = filepath;
                      metadata.type     = getAssetTypeFromFileExtension(filepath.extension().string());
        
        MG_CORE_ASSERT(metadata.type != AssetType::None);

        ref<Asset> asset = AssetImporter::import(handle, metadata);
        if (asset)
        {
            asset->assetHandle      = handle;
            m_loadedAssets [handle] = asset;
            m_assetRegistry[handle] = metadata;
            serializeAssetRegistry();

            return handle;
        }

        return 0; // invalid handle
    }

    const AssetMetadata& EditorAssetManager::getMetadata(AssetHandle handle) const
    {
        if (!m_assetRegistry.contains(handle))
        {
            return s_nullMetadata;
        }

        return m_assetRegistry.get(handle);
    }

    const std::filesystem::path& EditorAssetManager::getFilePath(AssetHandle handle) const
    {
        return getMetadata(handle).filepath;
    }

    AssetHandle EditorAssetManager::getAssetHandleByFilePath(const std::string& filepath) const
    {
        auto relativePath = std::filesystem::relative(filepath, Project::getActiveAssetDirectory());

        for (const auto& [handle, metadata] : m_assetRegistry)
        {
            if (metadata.filepath == relativePath)
            {
                return handle;
            }
        }

        return 0; // invalid handle
    }

    AssetType EditorAssetManager::getAssetTypeFromFileExtension(const std::string& extension)
    {
        if (!s_assetExtensionMap.contains(extension))
        {
            MG_CORE_WARN("Could not find AssetType for {}", extension);
            return AssetType::None;
        }

        return s_assetExtensionMap.at(extension);
    }

    ref<Asset> EditorAssetManager::getAsset(AssetHandle handle)
    {
        // 0. check if asset is memory only asset
        if (isMemoryOnlyAsset(handle))
        {
            return m_memoryAssets[handle];
        }

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

            asset = AssetImporter::import(handle, metadata);
            if (!asset) 
            {
                // import failed
                MG_CORE_ERROR("EditorAssetManager::getAsset - asset import failed!");
            }
            else
            {
                m_loadedAssets[handle] = asset;
            }
        }

        // 3. return asset
        return asset;
    }

    ref<Asset> EditorAssetManager::getAssetByName(const std::string& assetName)
    {
        AssetHandle handle = getAssetHandleByFilePath(assetName);
        
        return getAsset(handle);
    }

    void EditorAssetManager::serializeAssetRegistry()
    {
        // TODO: sorting asset registry when serializing!!

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

        auto path = Project::getActiveAssetRegistryPath().string();
        std::ofstream fout(path);
        if (fout.is_open())
        {
            fout << out.c_str();
        }

    }

    bool EditorAssetManager::deserializeAssetRegistry()
    {
        auto path = Project::getActiveAssetRegistryPath();

        // If AssetRegistry file doesn't exist, create one
        if (!std::filesystem::exists(path))
        {
            std::ofstream assetRegistryFile(path);
        }

        YAML::Node data;
        try
        {
            data = YAML::LoadFile(path.string());
        }
        catch (YAML::ParserException e)
        {
            MG_CORE_ERROR("Failed to load asset registry file '{}'\n    {}", path.string(), e.what());
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