#include "mgpch.h"
#include "AssetManager.h"

namespace mango
{

    bool EditorAssetManager::isAssetHandleValid(AssetHandle handle) const
    {
        return handle != 0 && m_assetRegistry.find(handle) != m_assetRegistry.end();
    }

    bool EditorAssetManager::isAssetLoaded(AssetHandle handle) const
    {
        return m_loadedAssets.find(handle) != m_loadedAssets.end();
    }

    const AssetMetadata& EditorAssetManager::getMetadata(AssetHandle handle) const
    {
        static AssetMetadata s_nullMetadata;
        auto it = m_assetRegistry.find(handle);
        if (it == m_assetRegistry.end())
        {
            return s_nullMetadata;
        }

        return it->second;
    }

    ref<Asset> EditorAssetManager::getAsset(AssetHandle handle) const
    {
        // 1. check if handle is valid
        if (!isAssetHandleValid(handle)) return nullptr;

        // 2. check if asset needs load (and if so, load)
        ref<Asset> asset = nullptr;
        if (isAssetLoaded(handle))
        {
            asset = m_loadedAssets.at(handle);
        }
        else
        {
            // load asset
            const AssetMetadata& metadata = getMetadata(handle);

            asset = AssetImporter::importAsset(handle, metadata);
            if (!asset) {} // import failed
        }

        // 3. return asset
        return asset;
    }

}