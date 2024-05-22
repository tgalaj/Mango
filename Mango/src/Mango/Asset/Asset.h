#pragma once

#include "AssetTypes.h"
#include "Mango/Core/Base.h"
#include "Mango/Core/UUID.h"

#include <string_view>

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
            return handle == other.handle;
        }

        virtual bool operator!=(const Asset& other) const
        {
            return handle != other.handle;
        }

    public:
        AssetHandle handle; // automatically generate handle (see UUID constructor)
    };
}