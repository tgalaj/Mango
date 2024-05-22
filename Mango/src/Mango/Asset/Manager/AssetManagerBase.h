#pragma once

#include "Mango/Asset/Asset.h"

#include <unordered_map>
#include <unordered_set>

namespace mango
{
    using AssetMap = std::unordered_map<AssetHandle, ref<Asset>>;

    class AssetManagerBase
    {
    public:
        virtual ref<Asset> getAsset          (AssetHandle handle)       = 0;
        virtual AssetType  getAssetType      (AssetHandle handle) const = 0;

        virtual void       addMemoryOnlyAsset(const ref<Asset>& asset, const std::string& assetName = "")  = 0;

        virtual bool       isAssetHandleValid(AssetHandle handle) const = 0;
        virtual bool       isAssetLoaded     (AssetHandle handle) const = 0;
        virtual bool       isMemoryOnlyAsset (AssetHandle handle) const = 0;
        virtual void       removeAsset       (AssetHandle handle)       = 0;

        virtual       std::unordered_set<AssetHandle> getAllAssetsOfType(AssetType type) const = 0;
        virtual const AssetMap&                       getLoadedAssets()                  const = 0;
        virtual const AssetMap&                       getMemoryOnlyAssets()              const = 0;

    };
}