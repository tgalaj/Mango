#include "mgpch.h"
#include "AssetImporter.h"

#include "TextureImporter.h"

#include <unordered_map>

namespace mango
{
    using AssetImportFunction = std::function<ref<Asset>(AssetHandle, const AssetMetadata&)>;

    static std::unordered_map<AssetType, AssetImportFunction> s_assetImportFunctions =
    {
        //{ AssetType::Scene,   nullptr },
        { AssetType::Texture, TextureImporter::importTexture2D }
    };

    ref<Asset> AssetImporter::importAsset(AssetHandle handle, const AssetMetadata& metadata)
    {
        if (s_assetImportFunctions.find(metadata.type) == s_assetImportFunctions.end())
        {
            MG_CORE_ERROR("No importer available for the asset type: {}", (uint16_t)metadata.type);
            return nullptr;
        }

        return s_assetImportFunctions.at(metadata.type)(handle, metadata);
    }
}