#pragma once

#include "AssetManagerBase.h"
#include "AssetMetadata.h"

#include <unordered_map>

namespace mango
{
    using AssetRegistry = std::unordered_map<AssetHandle, AssetMetadata>;

    class EditorAssetManager : public AssetManagerBase
    {
    public:
        virtual ref<Asset> getAsset          (AssetHandle handle) const override;

        virtual bool       isAssetHandleValid(AssetHandle handle) const override;
        virtual bool       isAssetLoaded     (AssetHandle handle) const override;
        virtual AssetType  getAssetType      (AssetHandle handle) const override;

        void importAsset(const std::filesystem::path& filepath);

        const AssetMetadata&         getMetadata(AssetHandle handle) const;
        const std::filesystem::path& getFilePath(AssetHandle handle) const;

        const AssetRegistry& getAssetRegistry() const { return m_assetRegistry; }

        void serializeAssetRegistry();
        bool deserializeAssetRegistry();
    private:
        AssetRegistry m_assetRegistry;
        AssetMap      m_loadedAssets;
        AssetMap      m_memoryAssets; // TODO: memory-only assets
    };
}