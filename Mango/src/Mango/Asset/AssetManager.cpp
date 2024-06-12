#pragma once
#include "mgpch.h"
#include "AssetManager.h"

namespace mango
{
    static std::unordered_map<AssetType, std::function<ref<Asset>()>> s_defaultAssetMap =
    {
        { AssetType::Texture, [](){  } },
    };

    ref<Asset> getDefaultAsset(AssetType type)
    {

    }
}