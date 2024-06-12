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

        template<typename T>
        static ref<T> getAssetByName(const std::string& name)
        {
            ref<Asset> asset = Project::getActive()->getAssetManager()->getAssetByName(name);
            return std::static_pointer_cast<T>(asset);
        }

        static ref<Asset> getDefaultAsset(AssetType type);


        static AssetType getAssetType(AssetHandle handle)
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

        static bool isMemoryOnlyAsset(AssetHandle handle)
        {
            return Project::getActive()->getAssetManager()->isMemoryOnlyAsset(handle);
        }

        void removeAsset(AssetHandle handle)
        {
            Project::getActive()->getAssetManager()->removeAsset(handle);
        }

        template<typename T>
        static void addMemoryOnlyAsset(const ref<Asset>& asset, const std::string& assetName = "")
        {
            static_assert(std::is_base_of_v<Asset, T>, "T must be a subclass of Asset");

            Project::getActive()->getAssetManager()->addMemoryOnlyAsset(asset, assetName);
        }

        static std::unordered_set<AssetHandle> getAllAssetsOfType(AssetType type)
        {
            return Project::getActive()->getAssetManager()->getAllAssetsOfType(type);
        }

        static const AssetMap& getLoadedAssets()
        {
            return Project::getActive()->getAssetManager()->getLoadedAssets();
        }

        static const AssetMap& getMemoryOnlyAssets()
        {
            return Project::getActive()->getAssetManager()->getMemoryOnlyAssets();
        }

        template<typename T, typename... Args>
        static AssetHandle createMemoryOnlyAsset(Args&&... args, const std::string& assetName = "")
        {
            static_assert(std::is_base_of_v<Asset, T>, "T must be a subclass of Asset");

            ref<T> asset         = createRef<T>(std::forward<Args>(args)...);
                   asset->handle = AssetHandle();

            Project::getActive()->getAssetManager()->addMemoryOnlyAsset<T>(asset, assetName);

            return asset->handle;
        }

    };
}