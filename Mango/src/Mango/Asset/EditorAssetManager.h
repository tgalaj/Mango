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

        const AssetMetadata& getMetadata(AssetHandle handle) const;
    private:
        AssetRegistry m_assetRegistry;
        AssetMap      m_loadedAssets;

        // TODO: memory-only assets
    };
}