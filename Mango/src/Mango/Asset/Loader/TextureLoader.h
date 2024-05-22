#pragma once

#include "Mango/Asset/Asset.h"
#include "Mango/Asset/AssetMetadata.h"

namespace mango
{
    class TextureLoader
    {
    public:
        // AssetMetadata filepath is relative to project asset directory
        static ref<Texture> importTexture2D(AssetHandle handle, const AssetMetadata& metadata);

        // Reads file directly from the filesystem
        static ref<Texture> loadTexture2D(const std::filesystem::path& path);
    };
}