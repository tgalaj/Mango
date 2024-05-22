#pragma once

#include "Manager/AssetManagerBase.h"

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
        
        static AssetType  getAssetType(AssetHandle handle)
        {
            return  Project::getActive()->getAssetManager()->getAssetType(handle);
        }

        static bool isAssetHandleValid(AssetHandle handle)
        {
            return Project::getActive()->getAssetManager()->isAssetHandleValid(handle);
        }
        
        static bool isAssetLoaded(AssetHandle handle)
        {
            return Project::getActive()->getAssetManager()->isAssetLoaded(handle);

        }

        
    };
}