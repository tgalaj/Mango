#pragma once

#include "Mango/Core/UUID.h"

namespace mango
{
    using AssetHandle = UUID;

    enum class AssetType
    {
        None = 0,
        Scene,
        Texture,
        Mesh // TODO(tgalaj): list
    };

    class Asset
    {
    public:
        AssetHandle handle; // automatically generate handle (see UUID constructor)

        virtual AssetType getAssetType() const = 0;
    };
}