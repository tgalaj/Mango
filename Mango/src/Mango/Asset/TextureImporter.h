#pragma once

#include "Asset.h"
#include "AssetMetadata.h"

namespace mango
{
    class TextureImporter
    {
    public:
        // AssetMetadata filepath is relative to project asset directory
        static ref<Texture> importTexture2D(AssetHandle handle, const AssetMetadata& metadata);

        // Reads file directly from the filesystem
        static ref<Texture> loadTexture2D(const std::filesystem::path& path);
    };
}