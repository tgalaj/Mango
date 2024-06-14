#pragma once

#include "AssetManagerBase.h"
#include "Mango/Asset/AssetImporter.h"
#include "Mango/Asset/AssetRegistry.h"
#include "Mango/Project/Project.h"
#include "Mango/Utils/Hash.h"

namespace mango
{
    class EditorAssetManager : public AssetManagerBase
    {
    public:
        EditorAssetManager();
        virtual ~EditorAssetManager() = default;

        ref<Asset> getAsset      (AssetHandle handle)           override;
        ref<Asset> getAssetByName(const std::string& assetName) override;
        AssetType  getAssetType  (AssetHandle handle)           override;

        void addAsset          (const ref<Asset>& asset, const std::filesystem::path& filepath      ) override;
        void addMemoryOnlyAsset(const ref<Asset>& asset, const std::string&           assetName = "") override;

        bool isAssetHandleValid(AssetHandle handle) override;
        bool isAssetLoaded     (AssetHandle handle) override;
        bool isMemoryOnlyAsset (AssetHandle handle) override;
        void removeAsset       (AssetHandle handle) override;

              std::unordered_set<AssetHandle> getAllAssetsOfType(AssetType type) override;
        const AssetMap&                       getLoadedAssets()                  override;
        const AssetMap&                       getMemoryOnlyAssets()              override;

        AssetHandle importAsset(const std::filesystem::path& filepath);

        const AssetMetadata& getMetadata(AssetHandle handle);
        
        std::filesystem::path getFilePath    (      AssetHandle            handle);
        std::filesystem::path getRelativePath(const std::filesystem::path& filepath);

        AssetHandle getAssetHandleByFilePath     (const std::string& filepath);
        AssetType   getAssetTypeFromFileExtension(const std::string& extension);


        const AssetRegistry& getAssetRegistry() const { return m_assetRegistry; }

        void serializeAssetRegistry();
        bool deserializeAssetRegistry();

    private:
        AssetRegistry m_assetRegistry;
        AssetMap      m_loadedAssets;
        AssetMap      m_memoryAssets;
    };
}