#pragma once

#include "Mango/Asset/Asset.h"

#include <map>

namespace mango
{
    using AssetMap = std::map<AssetHandle, ref<Asset>>;

    class AssetManagerBase
    {
    public:
        virtual ref<Asset> getAsset          (AssetHandle handle) = 0;

        virtual bool       isAssetHandleValid(AssetHandle handle) const = 0;
        virtual bool       isAssetLoaded     (AssetHandle handle) const = 0;
        virtual AssetType  getAssetType      (AssetHandle handle) const = 0;
        virtual void       removeAsset       (AssetHandle handle)       = 0;
    };
}