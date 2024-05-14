#pragma once

#include "AssetManagerBase.h"

#include "Mango/Project/Project.h"

namespace mango
{
    class AssetManager
    {
    public:
        template<typename T>
        static ref<T> getAsset(AssetHandle handle)
        {
            ref<Asset> asset = Project::getActive()->getAssetManager()->getAsset(handle);
            return std::static_pointer_cast<T>(asset);
        }
    };
}