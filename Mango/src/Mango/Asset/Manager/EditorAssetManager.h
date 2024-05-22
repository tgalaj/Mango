#pragma once

#include "AssetManagerBase.h"
#include "Mango/Asset/AssetImporter.h"
#include "Mango/Asset/AssetMetadata.h"
#include "Mango/Asset/AssetRegistry.h"
#include "Mango/Project/Project.h"
#include "Mango/Utils/Hash.h"

#include <unordered_map>

namespace mango
{
    class EditorAssetManager : public AssetManagerBase
    {
    public:
        EditorAssetManager();
        virtual ~EditorAssetManager() = default;

        virtual ref<Asset> getAsset          (AssetHandle handle) override;

        virtual bool       isAssetHandleValid(AssetHandle handle) const override;
        virtual bool       isAssetLoaded     (AssetHandle handle) const override;
        virtual AssetType  getAssetType      (AssetHandle handle) const override;
        virtual void       removeAsset       (AssetHandle handle) override;

        void importAsset(const std::filesystem::path& filepath);

        const AssetMetadata&         getMetadata(      AssetHandle            handle)   const;
        const AssetMetadata&         getMetadata(const std::filesystem::path& filepath) const;
        const std::filesystem::path& getFilePath(      AssetHandle            handle)   const;

        AssetHandle getAssetHandleFromFilePath   (const std::string& filepath) const;
        AssetType   getAssetTypeFromFileExtension(const std::string& extension);

        const AssetRegistry& getAssetRegistry() const { return m_assetRegistry; }

        void serializeAssetRegistry();
        bool deserializeAssetRegistry();
        
        template<typename T, typename... Args>
        ref<T> createAsset(const std::filesystem::path& filepath, Args&&... args)
        {
            static_assert(std::is_base_of<Asset, T>::value, "T must be a subclass of Asset");

            // Create the asset's metadata
            AssetMetadata metadata;
            metadata.handle   = AssetHandle();
            metadata.filepath = std::filesystem::relative(filepath, Project::getActiveAssetDirectory());
            metadata.type     = T::getStaticType();

            // Add the asset to the registry
            m_assetRegistry[metadata.handle] = metadata;
            serializeAssetRegistry();

            // Create the new asset
            ref<T> asset  = createRef<T>(std::forward<Args>(args)...);
            asset->handle = metadata.handle;

            m_loadedAssets[asset->handle] = asset;
            AssetImporter::serialize(metadata, asset);

            return asset;
        }

        template<typename T, typename... Args>
        ref<T> createMemoryOnlyAsset(const std::string& name, Args&&... args)
        {
            static_assert(std::is_base_of<Asset, T>::value, "T must be a subclass of Asset");

            // Create the asset's metadata
            AssetMetadata metadata;
            metadata.handle        = StringHash64(name);
            metadata.filepath      = name;
            metadata.type          = T::getStaticType();
            metadata.isMemoryAsset = true;

            // Add the asset to the registry
            m_assetRegistry[metadata.handle] = metadata;

            // Create the new asset
            ref<T> asset  = createRef<T>(std::forward<Args>(args)...);
            asset->handle = metadata.handle;

            m_memoryAssets[asset->handle] = asset;

            return asset;
        }

        template<typename T>
        static ref<T> getAsset(const std::string& filepath)
        {
            ref<Asset> asset = getAsset(getAssetHandleFromFilePath(filepath));
            return std::static_pointer_cast<T>(asset);
        }

    private:
        AssetRegistry m_assetRegistry;
        AssetMap      m_loadedAssets;
        AssetMap      m_memoryAssets; // TODO: memory-only assets
    };
}