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
        AssetType  getAssetType  (AssetHandle handle) const     override;

        void       addMemoryOnlyAsset(const ref<Asset>& asset, const std::string& assetName = "") override;

        bool       isAssetHandleValid(AssetHandle handle) const override;
        bool       isAssetLoaded     (AssetHandle handle) const override;
        bool       isMemoryOnlyAsset (AssetHandle handle) const override;
        void       removeAsset       (AssetHandle handle) override;

              std::unordered_set<AssetHandle> getAllAssetsOfType(AssetType type) const override;
        const AssetMap&                       getLoadedAssets()                  const override;
        const AssetMap&                       getMemoryOnlyAssets()              const override;

        void importAsset(const std::filesystem::path& filepath);

        const AssetMetadata&         getMetadata(AssetHandle handle) const;
        const std::filesystem::path& getFilePath(AssetHandle handle) const;

        AssetHandle getAssetHandleByFilePath     (const std::string& filepath) const;
        AssetType   getAssetTypeFromFileExtension(const std::string& extension);


        const AssetRegistry& getAssetRegistry() const { return m_assetRegistry; }

        void serializeAssetRegistry();
        bool deserializeAssetRegistry();
        
        template<typename T, typename... Args>
        ref<T> createAsset(const std::filesystem::path& filepath, Args&&... args)
        {
            static_assert(std::is_base_of_v<Asset, T>, "T must be a subclass of Asset");

            // Create the asset's metadata
            AssetHandle handle;

            AssetMetadata metadata;
            metadata.filepath = std::filesystem::relative(filepath, Project::getActiveAssetDirectory());
            metadata.type     = T::getStaticType();

            // Add the asset to the registry
            m_assetRegistry[handle] = metadata;
            serializeAssetRegistry();

            // Create the new asset
            ref<T> asset  = createRef<T>(std::forward<Args>(args)...);
            asset->handle = handle;

            m_loadedAssets[asset->handle] = asset;
            AssetImporter::serialize(metadata, asset);

            return asset;
        }

        template<typename T, typename... Args>
        ref<T> createMemoryOnlyAsset(const std::string& name, Args&&... args)
        {
            static_assert(std::is_base_of_v<Asset, T>, "T must be a subclass of Asset");

            // Create the asset's metadata
            AssetHandle handle {StringHash64(name)};

            AssetMetadata metadata;
            metadata.filepath      = name;
            metadata.type          = T::getStaticType();
            metadata.isMemoryOnlyAsset = true;

            // Add the asset to the registry
            m_assetRegistry[handle] = metadata;

            // Create the new asset
            ref<T> asset  = createRef<T>(std::forward<Args>(args)...);
            asset->handle = handle;

            m_memoryAssets[asset->handle] = asset;

            return asset;
        }

        template<typename T>
        ref<T> getAsset(const std::string& filepath)
        {
            ref<Asset> asset = getAsset(getAssetHandleByFilePath(filepath));
            return std::static_pointer_cast<T>(asset);
        }

    private:
        AssetRegistry m_assetRegistry;
        AssetMap      m_loadedAssets;
        AssetMap      m_memoryAssets; // TODO: memory-only assets
    };
}