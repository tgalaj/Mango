#include "mgpch.h"
#include "AssetRegistry.h"

namespace mango
{
    struct AssetMetadata;

    AssetMetadata& AssetRegistry::operator[](const AssetHandle handle)
    {
        return m_assetRegistry[handle];
    }

    AssetMetadata& AssetRegistry::get(const AssetHandle handle)
    {
        return m_assetRegistry.at(handle);
    }

    const AssetMetadata& AssetRegistry::get(const AssetHandle handle) const
    {
        return m_assetRegistry.at(handle);
    }

    bool AssetRegistry::contains(const AssetHandle handle) const
    {
        return m_assetRegistry.contains(handle);
    }

    size_t AssetRegistry::remove(const AssetHandle handle)
    {
        return m_assetRegistry.erase(handle);
    }

    void AssetRegistry::clear()
    {
        m_assetRegistry.clear();
    }

}