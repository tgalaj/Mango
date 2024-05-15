#pragma once

#include "Asset.h"
#include "Mango/Core/Base.h"

namespace mango
{
    using AssetMap = std::unordered_map<AssetHandle, ref<Asset>>;

    class AssetManagerBase
    {
    public:
        virtual ref<Asset> getAsset          (AssetHandle handle) const = 0;

        virtual bool       isAssetHandleValid(AssetHandle handle) const = 0;
        virtual bool       isAssetLoaded     (AssetHandle handle) const = 0;
        virtual AssetType  getAssetType      (AssetHandle handle) const = 0;

    };
}