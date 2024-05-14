#pragma once

#include "AssetMetadata.h"

namespace mango
{
    class AssetImporter
    {
    public:
        static ref<Asset> importAsset(AssetHandle handle, const AssetMetadata& metadata);
    };
}