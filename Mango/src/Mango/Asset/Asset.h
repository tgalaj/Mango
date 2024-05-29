#pragma once

#include "AssetTypes.h"
#include "Mango/Core/Base.h"
#include "Mango/Core/UUID.h"

namespace mango
{
    using AssetHandle = UUID;

    class Asset
    {
    public:
        virtual ~Asset() = default;

        static  AssetType getStaticType() { return AssetType::None; }
        virtual AssetType getAssetType () const = 0;

        virtual bool operator==(const Asset& other) const
        {
            return assetHandle == other.assetHandle;
        }

        virtual bool operator!=(const Asset& other) const
        {
            return assetHandle != other.assetHandle;
        }

    public:
        AssetHandle assetHandle; // automatically generate handle (see UUID constructor)
    };
}