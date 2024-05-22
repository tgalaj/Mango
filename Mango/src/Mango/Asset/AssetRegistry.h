#pragma once

#include "AssetMetadata.h"

#include <unordered_map>

namespace mango
{
    class AssetRegistry
    {
    public:
              AssetMetadata& operator[](const AssetHandle handle);
              AssetMetadata& get       (const AssetHandle handle);
        const AssetMetadata& get       (const AssetHandle handle) const;

        size_t size() const { return m_assetRegistry.size(); }
        
        bool   contains(const AssetHandle handle) const;
        size_t remove  (const AssetHandle handle);
        void   clear   ();

        auto begin()       { return m_assetRegistry.begin(); }
        auto end  ()       { return m_assetRegistry.end();   }
        auto begin() const { return m_assetRegistry.begin(); }
        auto end  () const { return m_assetRegistry.end();   }

    private:
        std::unordered_map<AssetHandle, AssetMetadata> m_assetRegistry;
    };
}