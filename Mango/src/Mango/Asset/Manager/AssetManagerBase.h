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
        virtual ref<Asset> getAsset          (AssetHandle handle)           = 0;
        virtual ref<Asset> getAssetByName    (const std::string& assetName) = 0;
        virtual AssetType  getAssetType      (AssetHandle handle)           = 0;

        virtual void addAsset          (const ref<Asset>& asset, const std::filesystem::path& filepath      ) = 0;
        virtual void addMemoryOnlyAsset(const ref<Asset>& asset, const std::string&           assetName = "") = 0;

        virtual bool isAssetHandleValid(AssetHandle handle) = 0; // check if the handle is valid (doesn't tell anything about the asset itself)
        virtual bool isAssetLoaded     (AssetHandle handle) = 0; // check if the asset is loaded
        virtual bool isMemoryOnlyAsset (AssetHandle handle) = 0;
        virtual void removeAsset       (AssetHandle handle) = 0;

        virtual       std::unordered_set<AssetHandle> getAllAssetsOfType(AssetType type) = 0;
        virtual const AssetMap&                       getLoadedAssets()                  = 0;
        virtual const AssetMap&                       getMemoryOnlyAssets()              = 0;

    };
}